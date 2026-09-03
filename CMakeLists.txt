cmake_minimum_required(VERSION 3.24)

project(IBCImageCompressor VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.6 REQUIRED COMPONENTS Quick QuickControls2 Concurrent Gui Test)
qt_standard_project_setup(REQUIRES 6.6)

qt_add_executable(IBCImageCompressor
    src/main.cpp
    src/ImageCompressor.cpp
    src/ImageCompressor.h
)

qt_add_qml_module(IBCImageCompressor
    URI IBC.Compressor
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/components/GlowButton.qml
        qml/components/GlassCard.qml
    RESOURCES
        assets/ibc-compressor.svg
)

qt_add_shaders(IBCImageCompressor "ibc_holographic_shaders"
    BATCHABLE
    PRECOMPILE
    OPTIMIZED
    PREFIX "/shaders"
    FILES shaders/holographic.frag
)

target_link_libraries(IBCImageCompressor PRIVATE
    Qt6::Quick Qt6::QuickControls2 Qt6::Concurrent Qt6::Gui
)

target_compile_definitions(IBCImageCompressor PRIVATE
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_TO_ASCII
)

set_target_properties(IBCImageCompressor PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME "IBC Compressor de Imagem"
    MACOSX_BUNDLE_GUI_IDENTIFIER "br.com.ibchertz.imagecompressor"
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS IBCImageCompressor
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_qml_app_script(
    TARGET IBCImageCompressor
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

include(CTest)
if(BUILD_TESTING)
    qt_add_executable(IBCCompressorTests
        tests/ImageCompressorTest.cpp
        src/ImageCompressor.cpp
        src/ImageCompressor.h
    )
    target_include_directories(IBCCompressorTests PRIVATE src)
    target_link_libraries(IBCCompressorTests PRIVATE
        Qt6::Gui Qt6::Concurrent Qt6::Test
    )
    add_test(NAME compression_engine COMMAND IBCCompressorTests)
endif()
