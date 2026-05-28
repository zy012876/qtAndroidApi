# PRD: Test Suite, Error Retry, and Back Navigation Icons

## Problem Statement / 问题陈述

The app has zero automated tests — all provider parsing, database operations, and model logic are untested. A regression in SSE chunk parsing or a migration bug only surfaces when a user hits it. / 应用零自动化测试——所有 provider 解析、数据库操作、模型逻辑未经测试。流解析回归或迁移 bug 只有用户踩到才会发现。

When an API request fails, the user's message is lost and replaced with error text in the chat bubble — there is no way to retry. / API 请求失败时，用户消息被丢失，聊天气泡显示错误文字——无法重试。

Page navigation uses a rotated delete/trash icon (`delete.svg`) instead of a proper back arrow — confusing and semantically wrong. / 页面导航用旋转的删除图标充当返回按钮——令人困惑且语义错误。

## Solution / 解决方案

Add a targeted test suite covering three critical modules using Qt Test. / 用 Qt Test 为三个关键模块添加测试套件。

Add `retryLastMessage()` to ChatManager — when a send fails, the assistant bubble shows "Tap to retry" and re-sends the last user message. / ChatManager 添加 `retryLastMessage()`——发送失败时助手气泡显示"点击重试"，重新发送上一条用户消息。

Replace navigation back buttons with a proper back arrow icon and ensure the UI is clear on which button does what. / 用真正的返回箭头图标替换导航返回按钮。

## User Stories / 用户故事

1. As a developer, I want `MessageListModel` unit tests, so that I can refactor message handling without breaking streaming, appending, or clearing behavior. / 作为开发者，我想要 `MessageListModel` 单元测试，以便重构消息处理时不会破坏流式、追加或清空行为。

2. As a developer, I want `DatabaseManager` integration tests against an in-memory SQLite database, so that I can verify schema migrations and CRUD operations without depending on a real device database. / 作为开发者，我想要 `DatabaseManager` 基于内存 SQLite 的集成测试，以便验证 schema 迁移和 CRUD 操作而不依赖真实设备数据库。

3. As a developer, I want SSE parsing tests (using OpenAI as the reference provider), so that chunk splitting, partial-line buffering, and content extraction are verified under controlled input. / 作为开发者，我想要 SSE 解析测试（以 OpenAI 为参考 provider），以便在受控输入下验证 chunk 分割、部分行缓冲和内容提取。

4. As a user, I want to retry a failed message without re-typing it, so that a transient network error doesn't force me to re-compose my question. / 作为用户，我希望能重试失败的消息而不用重新打字，这样临时网络错误不会强迫我重写问题。

5. As a user, I want back navigation buttons to show an arrow icon, not a trash can, so that I can tell at a glance which button takes me to the previous page. / 作为用户，我希望返回按钮显示箭头图标而非垃圾桶，这样我一眼就知道哪个按钮是返回上一页。

## Implementation Decisions / 实现决策

### Module: Test Infrastructure / 测试基础设施
- Add a `test/` directory at repo root with its own `CMakeLists.txt`, included via `add_subdirectory` from the root CMakeLists. / 在仓库根目录添加 `test/` 目录，自带 `CMakeLists.txt`，通过根 CMakeLists 的 `add_subdirectory` 引入。
- Use Qt Test (`QTestLib`) — already bundled with Qt6::Core, zero new dependencies. / 使用 Qt Test（`QTestLib`）——Qt6::Core 自带，零新依赖。
- Link tests against a static library target of the app's source files (via `qt_add_library`) so tests share compilation units without duplicating build config. / 测试链接应用源码的静态库目标（通过 `qt_add_library`），共享编译单元而不重复构建配置。

### Module: MessageListModel Tests / MessageListModel 单元测试
- Test `appendMessage`, `prependMessages`, `clear`, `setMessages`, `updateLastAssistantMessage`, `finalizeLastAssistantMessage`, `allMessages`, and `count`. / 测试以上方法。
- Verify role data returns correct values for each role constant. / 验证每个 role 常量返回正确数据。
- Verify `countChanged` signal is emitted on append and clear. / 验证 append 和 clear 时发出 `countChanged` 信号。
- Deep module: `MessageListModel` is pure in-memory data — the simplest and most valuable test target. / `MessageListModel` 是纯内存数据——最简单也最有价值的测试目标。

### Module: DatabaseManager Tests / DatabaseManager 集成测试
- Use `:memory:` SQLite database to avoid filesystem dependencies. / 使用 `:memory:` SQLite 数据库避免文件系统依赖。
- Test schema migration: verify version 0 → 1 path creates all tables. / 测试 schema 迁移：验证 version 0→1 路径创建所有表。
- Test CRUD: create conversation, insert messages, load messages ordered by timestamp, load all conversations ordered by updated_at DESC, update conversation metadata, delete conversation cascades to messages. / 测试增删改查：创建对话、插入消息、按时间戳排序加载消息、按 updated_at DESC 加载所有对话、更新对话元数据、删除对话级联删除消息。

### Module: SSE Parser Test / SSE 解析测试
- Test `OpenAiProvider::processResponseChunk` with pre-crafted byte arrays. / 用预制 byte array 测试 `OpenAiProvider::processResponseChunk`。
- Verify: single complete line, split across chunks, empty data, `[DONE]` marker, missing content in delta, multiple deltas in one chunk. / 验证：单完整行、跨 chunk 分割、空 data、`[DONE]` 标记、delta 中缺少 content、单 chunk 含多个 delta。
- Verify `responseChunkReceived` signal emitted with correct content fragments. / 验证 `responseChunkReceived` 信号发出正确的内容片段。

### Module: Error Retry / 错误重试
- Add `retryLastMessage()` to `ChatManager` — re-sends the last user message by finding it in `m_currentMessages`, then calling `sendMessage` with that text after removing the failed assistant bubble. / ChatManager 添加 `retryLastMessage()`——从 `m_currentMessages` 中找到最后一条用户消息，移除失败的助手气泡后以相同文本调用 `sendMessage`。
- In `onProviderError`, instead of finalizing with "Error: ..." text, finalize the streaming message with a special marker (empty content + a `retry` role or property). The QML delegate shows "Send failed. Tap to retry." and emits a retry signal. / `onProviderError` 中不写 "Error: ..."，而是用一个特殊标记结束流式消息。QML delegate 显示"发送失败，点击重试"并发起重试信号。
- The retry bubble must not be persisted to the database — only user and assistant messages with real content are persisted. / 重试气泡不持久化到数据库——只持久化有实际内容的用户和助手消息。

### Module: Back Navigation Icons / 返回导航图标
- Add a back arrow SVG icon (`back.svg`) to the resources. / 向资源添加返回箭头 SVG 图标（`back.svg`）。
- Replace `"qrc:/icons/delete.svg"` with `"qrc:/icons/back.svg"` in `SettingsPage.qml:25`, `ConversationsPage.qml:27`, and `ApiKeyManagementPage.qml:43`. / 在上述三处替换图标。
- Remove the `rotation: 180` hack on those buttons. / 去掉那些按钮上的 `rotation: 180` 权宜做法。

## Testing Decisions / 测试决策

What makes a good test: verify external behavior — given this input, the module produces that output or emits that signal. Don't test internal state or private implementation. / 什么是好的测试：验证外部行为——给定输入，模块产生什么输出或发出什么信号。不测内部状态或私有实现。

Modules tested / 测试的模块:
- `MessageListModel` — unit tests (deep module, zero deps) / 单元测试（深度模块，零依赖）
- `DatabaseManager` — integration tests (in-memory SQLite) / 集成测试（内存 SQLite）
- `OpenAiProvider::processResponseChunk` — unit test (mock network) / 单元测试（mock 网络）

Prior art: this project has no existing tests. The Qt Test pattern is standard Qt practice — `QTEST_MAIN`, `QVERIFY`, `QCOMPARE`, `QSignalSpy`. / 本项目没有已有测试。Qt Test 模式是 Qt 标准实践。

The test target should be runnable with `ctest` and as a standalone executable. / 测试目标应可通过 `ctest` 及独立可执行文件运行。

## Out of Scope / 不在范围内

- API key storage security hardening / API key 存储安全加固
- Light/dark theme support / 浅色/深色主题支持
- Provider registry refactor / Provider 注册机制重构
- Conversation history search / 对话历史搜索
- Conversation rename UX / 对话重命名体验
- Message edit or delete / 消息编辑或删除
- System prompt UI / System prompt 界面

## Further Notes / 补充说明

The SSE parser test should use OpenAI as the reference provider because its SSE format (OpenAI-compatible chunks) is also used by DeepSeek — testing one covers most of the other. Anthropic and Gemini have distinct formats that can be tested later. / SSE 解析测试以 OpenAI 为参考 provider，因为它的 SSE 格式（OpenAI 兼容 chunk）也被 DeepSeek 使用——测一个覆盖另一个。Anthropic 和 Gemini 格式不同，可后续再测。
