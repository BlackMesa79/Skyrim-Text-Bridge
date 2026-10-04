# Skyrim Text Bridge 0.3.9 测试版

新增默认关闭的“文本框聚焦时自动开启”设置，可通过 SKSE Menu Framework 保存，或在用户 INI 的 [Input] 下设置 AutoEnable=1。旧用户配置默认关闭，F8 和自定义快捷键保持可用。

- SkyUI：仅识别 InventoryMenu、MagicMenu、ContainerMenu、BarterMenu 中已启用文字输入且实际聚焦的可编辑 Scaleform 字段。
- Meridian/Tailor：通过独立 DOM 焦点报告识别字段；只读、禁用、密码、失去网页焦点或无法访问的跨域 iframe 不启用。
- 自动会话失焦即关闭，切换字段清除旧组合与队列；异步捕获校验字段身份，旧观察回报不能覆盖新会话。
- 手动关闭后尊重当前焦点，离开再聚焦才自动开启。菜单重置保留手动关闭状态；安装失败或外部重置不在同一焦点上循环重试。
- Modex 保持手动，Prisma 始终使用原生输入并避让。设置页、关闭模组、游戏失焦时停止自动观察。

保留当前工作目录已有 0.3.8 的编辑快捷键、待提交内容保护及 FavoriteWheel 可选输入状态/重置 API。补充 Media Keys Fix SKSE 的实际输入前置说明；不要求安装 Prisma。

此版本只提供本地测试包，尚未发布新的 GitHub/Nexus 正式版本。离线构建与回归检查通过不代表游戏实测通过，1.5.97 也仍待实测。安装 ZIP 仅含 DLL、默认 INI 和合并使用/许可说明的 readme.txt。
