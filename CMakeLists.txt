cmake_minimum_required(VERSION 3.21)

project(qtAndroidApi
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS
    Core
    Quick
    QuickControls2
    Network
    Sql
)

if(ANDROID)
    message(STATUS "Building for Android")
    set(ANDROID_SOURCES
        src/android/AndroidKeyStore.cpp
    )
    add_definitions(-DQ_OS_ANDROID)
elseif(WIN32)
    message(STATUS "Building for Windows (Desktop/Development)")
    set(DESKTOP_SOURCES
        src/android/DesktopKeyStore.cpp
    )
endif()

qt_add_executable(${PROJECT_NAME}
    src/main.cpp
    src/providers/AiProviderBase.cpp
    src/providers/OpenAiProvider.cpp
    src/providers/AnthropicProvider.cpp
    src/providers/GeminiProvider.cpp
    src/providers/DeepSeekProvider.cpp
    src/providers/ProviderFactory.cpp
    src/models/MessageListModel.cpp
    src/models/ConversationListModel.cpp
    src/managers/ChatManager.cpp
    src/managers/SettingsManager.cpp
    src/managers/DatabaseManager.cpp
    src/ThemeProvider.cpp
    ${ANDROID_SOURCES}
    ${DESKTOP_SOURCES}
    resources/resources.qrc
)

qt_add_qml_module(${PROJECT_NAME}
    URI "AIChat"
    VERSION 1.0
    QML_FILES
        qml/main.qml
        qml/pages/ChatPage.qml
        qml/pages/ConversationsPage.qml
        qml/pages/SettingsPage.qml
        qml/pages/ApiKeyManagementPage.qml
        qml/components/MessageBubble.qml
        qml/components/StreamingMessageBubble.qml
        qml/components/ChatInputBar.qml
        qml/components/ProviderChipSelector.qml
        qml/components/ThinkingIndicator.qml
        qml/components/ConversationListItem.qml
        qml/components/MarkdownLabel.qml
        qml/delegates/MessageDelegate.qml
        qml/delegates/ConversationDelegate.qml
        qml/dialogs/NewConversationDialog.qml
        qml/dialogs/DeleteConfirmDialog.qml
        qml/dialogs/ApiKeyDialog.qml
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Network
    Qt6::Sql
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(ANDROID)
    qt_android_generate_deployment_settings(${PROJECT_NAME})
    set_target_properties(${PROJECT_NAME} PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/android"
    )
    qt_android_add_apk_target(${PROJECT_NAME})
endif()

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
