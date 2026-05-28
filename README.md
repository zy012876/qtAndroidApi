# AI Chat / AI 聊天应用

A Qt6/QML multi-vendor AI chat application for Android. Chat with OpenAI, Anthropic Claude, Google Gemini, and DeepSeek — all in one app. / 一个基于 Qt6/QML 的多供应商 AI 聊天 Android 应用。一个应用中即可与 OpenAI、Anthropic Claude、Google Gemini 和 DeepSeek 对话。

## Features / 功能

- **Multi-vendor support / 多供应商支持** — OpenAI (GPT-4o, GPT-4o-mini, GPT-4-turbo, GPT-3.5-turbo), Anthropic Claude (Claude Sonnet 4, Claude 3.5 Sonnet/Haiku, Claude 3 Opus), Google Gemini (2.5 Flash/Pro, 1.5 Pro/Flash), and DeepSeek (V4 Pro, Chat, Reasoner)
- **Streaming responses / 流式回复** — Real-time token-by-token display with typing indicator / 逐 token 实时显示，带输入指示器
- **Conversation history / 对话历史** — All chats persist locally via SQLite, auto-titled from your first message / 所有对话通过 SQLite 本地持久化，自动以首条消息命名
- **Model switching / 模型切换** — Change model per conversation on the fly / 随时为每个对话切换模型
- **Dark theme / 深色主题** — Dark color scheme optimized for readability / 为可读性优化的深色配色方案

## Screenshots / 截图

| Chat / 聊天 | Conversations / 对话列表 | Settings / 设置 |
|---|---|---|
| Chat with any AI model / 与任意 AI 模型对话 | Browse and switch conversations / 浏览和切换对话 | Manage API keys per provider / 管理各供应商 API Key |

## Architecture / 架构

```
qml/           QML UI layer / QML 界面层
  pages/       ChatPage, ConversationsPage, SettingsPage, ApiKeyManagementPage
  components/  MessageBubble, ChatInputBar, ProviderChipSelector, etc.
  delegates/   MessageDelegate, ConversationDelegate
  dialogs/     ApiKeyDialog, NewConversationDialog, DeleteConfirmDialog
src/
  providers/   AiProviderBase → OpenAI, Anthropic, Gemini, DeepSeek
  managers/    ChatManager, SettingsManager, DatabaseManager
  models/      MessageListModel, ConversationListModel
  android/     AndroidKeyStore / DesktopKeyStore (API key storage)
android/       Android manifest, keystore helper
```

## Prerequisites / 环境要求

- **Qt 6.5+** with modules: Core, Quick, QuickControls2, Network, Sql
- **CMake 3.21+**
- **C++17** compiler
- **Android SDK/NDK** (for APK builds)
- API keys from at least one provider (get them at the links below)

| Provider / 供应商 | Get API Key / 获取 API Key |
|---|---|
| OpenAI | https://platform.openai.com/api-keys |
| Anthropic | https://console.anthropic.com/ |
| Google Gemini | https://aistudio.google.com/apikey |
| DeepSeek | https://platform.deepseek.com/api_keys |

## Build & Run / 构建与运行

### Desktop (Windows development) / 桌面端（Windows 开发）

```bash
cmake -B build -DCMAKE_PREFIX_PATH=<path-to-qt6>
cmake --build build
./build/qtAndroidApi
```

### Android / 安卓

Open the project in Qt Creator, configure an Android kit, and deploy. Or build from command line: / 在 Qt Creator 中打开项目，配置 Android 工具链后部署。或命令行构建：

```bash
cmake -B build -DCMAKE_PREFIX_PATH=<path-to-qt6-android> \
      -DANDROID_SDK_ROOT=<path-to-android-sdk> \
      -DANDROID_NDK_ROOT=<path-to-android-ndk> \
      -DCMAKE_TOOLCHAIN_FILE=<path-to-qt6>/lib/cmake/Qt6/qt.toolchain.cmake
cmake --build build --target apk
```

## Usage / 使用说明

1. **Add API keys / 添加 API Key** — Open Settings, tap "Add" on a provider, paste your key / 打开设置，点击供应商旁的"Add"，粘贴你的 API Key
2. **Start a chat / 开始聊天** — Select a provider chip at the top, type a message, hit send / 选择顶部的供应商标签，输入消息，发送
3. **Switch models / 切换模型** — Use the model dropdown above the chat / 使用聊天区域上方的模型下拉菜单
4. **Browse history / 浏览历史** — Tap the history icon (top-left) to see past conversations / 点击左上角历史图标查看过往对话
5. **New conversation / 新建对话** — Switch providers or tap "+" in the history page / 切换供应商或在历史页点击"+"

## Supported Models / 支持模型

| Provider | Models |
|---|---|
| OpenAI | gpt-4o, gpt-4o-mini, gpt-4-turbo, gpt-3.5-turbo |
| Anthropic | claude-sonnet-4-20250514, claude-3-5-sonnet-20241022, claude-3-opus-20240229, claude-3-5-haiku-20241022 |
| Gemini | gemini-2.5-flash, gemini-2.5-pro, gemini-1.5-pro, gemini-1.5-flash |
| DeepSeek | deepseek-v4-pro, deepseek-chat, deepseek-reasoner |

## License / 许可证

MIT
