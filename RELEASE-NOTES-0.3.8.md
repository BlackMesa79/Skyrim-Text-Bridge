# 0.3.8 — 原生文本编辑器兼容

为 FavoriteWheel 0.2.7 等独立原生 UI 提供可选 C ABI：`SkyrimTextBridge_QueryInputV1(version, hotkey)` 返回支持/启用/捕获/候选或待提交状态，`SkyrimTextBridge_ResetInputV1()` 清理输入会话。文本继续通过游戏 Unicode CharEvent 注入，不共享 ImGui context、STL 对象或游戏指针。

Ctrl+A/C/X/V 在非组合状态下作为编辑命令传递；Ctrl+Space 仍由输入法处理中英文切换。输入确认键在提交文字尚未排出队列时保持抑制，避免同一次确认同时提交 UI 表单。

真实游戏内中文候选、hook 加载顺序与轮盘编辑仍需实测。原有 Prisma/Meridian 支持不新增硬依赖。
