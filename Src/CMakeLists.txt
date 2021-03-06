cmake_minimum_required(VERSION 2.8)

if(POLICY CMP0083)
    cmake_policy(SET CMP0083 NEW)
endif()

if(POLICY CMP0020)
    cmake_policy(SET CMP0020 NEW)
endif()

project(FaceRecognizer)

OPTION(HAVE_PERFORMANCE "Set to ON to build performance)." OFF)
if(HAVE_PERFORMANCE)
    set(LIBS_DEFINES ${LIBS_DEFINES} HAVE_PERFORMANCE)
    set(SOURCES_FILES ${SOURCES_FILES} Performance.cpp)
endif()

option(USE_YUV "Use libyuv" ON)
if(USE_YUV)
    #Use: https://github.com/KangLin/libyuv
    find_package(YUV)
    if(YUV_FOUND)
        set(LIBS_DEFINES ${LIBS_DEFINES} HAVE_LIBYUV)
        set(LIBS ${LIBS} YUV)
        if(ANDROID)
            INSTALL(FILES $<TARGET_FILE:YUV>
                    DESTINATION "libs/${ANDROID_ABI}")
        else()
            INSTALL(FILES $<TARGET_FILE:YUV>
                    DESTINATION "${CMAKE_INSTALL_BINDIR}")
        endif()
    endif()
endif(USE_YUV)

option(USE_FFMPEG "Use ffmpeg" ON)
if(USE_FFMPEG)
    find_package(FFMPEG)
endif(USE_FFMPEG)

option(USE_OPENCV "Use opencv" ON)
if(USE_OPENCV)
    find_package(OpenCV)
    if(OpenCV_FOUND)
        set(LIBS_DEFINES ${LIBS_DEFINES} HAVE_OPENCV OpenCV_VERSION_MAJOR=${OpenCV_VERSION_MAJOR})
        set(LIBS ${LIBS} ${OpenCV_LIBS})
        set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
    
        #android 编译出来的是静态库
        foreach(opencv_component ${OpenCV_LIBS})
            if(ANDROID)
                INSTALL(FILES $<TARGET_FILE:${opencv_component}>
                    DESTINATION "libs/${ANDROID_ABI}"
                    COMPONENT Runtime)
            else()
                INSTALL(FILES $<TARGET_FILE:${opencv_component}>
                    DESTINATION "${CMAKE_INSTALL_BINDIR}"
                    COMPONENT Runtime)
            endif()
        endforeach()
        
    endif()
endif(USE_OPENCV)

find_package(SeetaFace)
if(SeetaFace_FOUND)
    set(LIBS_DEFINES ${LIBS_DEFINES} HAVE_SEETA_FACE)
    set(LIBS ${LIBS} ${SeetaFace_LIBRARIES})
    foreach(componet_seetaFace ${SeetaFace_LIBRARIES})
        if(ANDROID)
            INSTALL(FILES $<TARGET_FILE:${componet_seetaFace}>
                DESTINATION "libs/${ANDROID_ABI}"
                COMPONENT Runtime)
        else()
            INSTALL(FILES $<TARGET_FILE:${componet_seetaFace}>
                DESTINATION "${CMAKE_INSTALL_BINDIR}"
                COMPONENT Runtime)
        endif()
    endforeach()
    
    set(SOURCES_FILES
        ${SOURCES_FILES}
        Seeta/FaceSeeta.cpp
        Seeta/DetectorSeeta.cpp
        Seeta/TrackerSeeta.cpp
        Seeta/LandmarkerSeeta.cpp
        Seeta/RecognizerSeeta.cpp
        Seeta/FaceToolsSeeta.cpp
        )
    
    set(HEADER_FILES
        ${HEADER_FILES}
        Seeta/FaceSeeta.h
        Seeta/DetectorSeeta.h
        Seeta/TrackerSeeta.h
        Seeta/LandmarkerSeeta.h
        Seeta/RecognizerSeeta.h
        Seeta/FaceToolsSeeta.h
        )
    
    set(SOURCE_UI_FILES
        ${SOURCE_UI_FILES}
        )
    
endif()

set(SOURCES_FILES
    ${SOURCES_FILES}
    CameraQtCaptureVideoFrame.cpp
    ImageTool.cpp
    Log.cpp
    ParameterFactory.cpp
    Parameter.cpp
    ParameterDetector.cpp
    ParameterLandmark.cpp
    ParameterRecognizer.cpp
    ParameterFaceTools.cpp
    Factory.cpp
    Face.cpp
    Detector.cpp
    Tracker.cpp
    Landmarker.cpp
    Recognizer.cpp
    FaceTools.cpp
    database/Database.cpp
    database/DataRegister.cpp
    database/TableRegister.cpp
    )

set(HEADER_FILES
    ${HEADER_FILES}
    CameraQtCaptureVideoFrame.h
    ImageTool.h
    Log.h
    ParameterFactory.h
    Parameter.h
    ParameterDetector.h
    ParameterLandmark.h
    ParameterRecognizer.h
    ParameterFaceTools.h
    Factory.h
    Face.h
    Detector.h
    Tracker.h
    Landmarker.h
    Recognizer.h
    FaceTools.h
    database/Database.h
    database/DataRegister.h
    database/TableRegister.h
    Performance.h
    )

#翻译
SET(TRANSLATIONS_NAME ${PROJECT_NAME})
include(${CMAKE_SOURCE_DIR}/cmake/Translations.cmake)

SET(RCC_FILES
    ../Resource/Resource.qrc
    ${TRANSLATIONS_RESOURCE_FILES})

string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
if(BUILD_TYPE STREQUAL "debug")
    set(LIBS_DEFINES ${LIBS_DEFINES} DEBUG)
    SET(RCC_FILES ${RCC_FILES} ../Resource/ResourceSql.qrc)
endif()

add_library(${PROJECT_NAME} ${SOURCES_FILES} ${HEADER_FILES} ${RCC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    PUBLIC_HEADER "${INSTALLHEADER_FILES}" # Install head files
    VERSION ${BUILD_VERSION})

target_compile_definitions(${PROJECT_NAME} PUBLIC RABBITCOMMON ${LIBS_DEFINES})
target_link_libraries(${PROJECT_NAME} PUBLIC RabbitCommon ${QT_LIBRARIES} ${LIBS})
target_include_directories(${PROJECT_NAME}
    PRIVATE ${RabbitCommon_DIR}/Src
    ${CMAKE_BINARY_DIR}
    ${INCLUDE_DIRS})

if(BUILD_VERSION)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DBUILD_VERSION="${BUILD_VERSION}")
    set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${BUILD_VERSION})
endif()

GENERATE_EXPORT_HEADER(${PROJECT_NAME})
string(TOLOWER ${PROJECT_NAME} LOWER_PROJECT_NAME)
set(INSTALLHEADER_FILES ${INSTALLHEADER_FILES} 
    ${CMAKE_CURRENT_BINARY_DIR}/${LOWER_PROJECT_NAME}_export.h)
file(COPY ${CMAKE_CURRENT_BINARY_DIR}/${LOWER_PROJECT_NAME}_export.h
    DESTINATION ${CMAKE_BINARY_DIR})

if(FFMPEG_FOUND)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ${FFMPEG_DEFINITIONS})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${FFMPEG_LIBRARIES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${FFMPEG_INCLUDE_DIRS})
endif()

if(ANDROID)
    # Install target
    INSTALL(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}Config
            RUNTIME DESTINATION "libs/${ANDROID_ABI}"
                COMPONENT Runtime
            LIBRARY DESTINATION "libs/${ANDROID_ABI}"
                COMPONENT Runtime
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/FaceReconginzer
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            )
    # Install sql file
    INSTALL(FILES ../Resource/db/database.sql DESTINATION assets/data/db
        COMPONENT Runtime)
    # Install model files
    if(EXISTS "${CMAKE_SOURCE_DIR}/model/Seeta")
        INSTALL(FILES ${CMAKE_SOURCE_DIR}/model/Seeta/fd_2_00.dat
            DESTINATION assets/model/Seeta
            COMPONENT Runtime)
        INSTALL(FILES ${CMAKE_SOURCE_DIR}/model/Seeta/fr_2_10.dat
            DESTINATION assets/model/Seeta
            COMPONENT Runtime)
        INSTALL(FILES ${CMAKE_SOURCE_DIR}/model/Seeta/pd_2_00_pts5.dat
            DESTINATION assets/model/Seeta
            COMPONENT Runtime)
        INSTALL(FILES ${CMAKE_SOURCE_DIR}/model/Seeta/pd_2_00_pts81.dat
            DESTINATION assets/model/Seeta
            COMPONENT Runtime)
    endif()
else()
    
    # Install target
    if(WIN32)
        INSTALL(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}Config
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
                COMPONENT Runtime
            LIBRARY DESTINATION "${CMAKE_INSTALL_BINDIR}"
                COMPONENT Runtime
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/FaceReconginzer
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            )
    else()
        INSTALL(TARGETS ${PROJECT_NAME}
            EXPORT ${PROJECT_NAME}Config
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
                COMPONENT Runtime
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
                COMPONENT Runtime
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/FaceReconginzer
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            )
    endif()
    # Install sql file
    INSTALL(FILES ../Resource/db/database.sql DESTINATION data/db)
    # Install model files
    if(EXISTS "${CMAKE_SOURCE_DIR}/model/Seeta")
        INSTALL(FILES ${CMAKE_SOURCE_DIR}/model/Seeta/fd_2_00.dat
            DESTINATION model/Seeta
            COMPONENT Runtime)
        INSTALL(FILES ${CMAKE_SOURCE_DIR}/model/Seeta/fr_2_10.dat
            DESTINATION model/Seeta
            COMPONENT Runtime)
        INSTALL(FILES ${CMAKE_SOURCE_DIR}/model/Seeta/pd_2_00_pts5.dat
            DESTINATION model/Seeta
            COMPONENT Runtime)
        INSTALL(FILES ${CMAKE_SOURCE_DIR}/model/Seeta/pd_2_00_pts81.dat
            DESTINATION model/Seeta
            COMPONENT Runtime)
    endif()
endif()

export(TARGETS ${PROJECT_NAME}
       APPEND FILE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake
)
# Install cmake configure files
install(EXPORT ${PROJECT_NAME}Config
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake"
        )
write_basic_package_version_file(
    "${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${BUILD_VERSION}
    COMPATIBILITY AnyNewerVersion)
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake")

IF("Release" STREQUAL CMAKE_BUILD_TYPE AND WIN32 AND BUILD_SHARED_LIBS)
    IF(CMAKE_BUILD_TYPE MATCHES Release AND MINGW)
        #windeployqt 分发时，是根据是否 strip 来判断是否是 DEBUG 版本,而用mingw编译时,qt没有自动 strip
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND strip "$<TARGET_FILE:${PROJECT_NAME}>"
            )
    ENDIF()

    #注意 需要把 ${QT_INSTALL_DIR}/bin 加到环境变量PATH中  
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${QT_INSTALL_DIR}/bin/windeployqt"
        --compiler-runtime
        --verbose 7
        "$<TARGET_FILE:${PROJECT_NAME}>"
        )
    #INSTALL(DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>/" DESTINATION "${CMAKE_INSTALL_BINDIR}")
ENDIF()

message("== HAVE_PERFORMANCE:${HAVE_PERFORMANCE}")
message("== OpenCV:${OpenCV_FOUND}")
message("== FFMPGE:${FFMPEG_FOUND}")
message("== LIBYUV:${YUV_FOUND}")
message("== SeetaFac:${SeetaFace_FOUND}")
