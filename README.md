# ShaderInPluginDemo UE4.16,4.23
![](https://github.com/sitonmoon/ShaderInPluginDemo/demo.png)

ShaderActor自定义Shader插件学习版 
适用引擎版本：UE4.16,4.23（下载版和源码版均可以）

更新日志：
UE4.16:
初始版本，务必将Shader文件夹中的.usf文件放到引擎的Shaders目录下！！

UE4.23：
已支持shader虚拟目录，不必拷贝Shader文件到引擎目录

插件使用指南：
将插件目录拷贝到项目目录的Plugins文件夹(没有请新建)，打开引擎，在内容浏览器右下角选择显示插件内容，会看到ShaderActor文件夹 拖入BPShaderActor蓝图类(为一个面片) 运行即可看到Shader运行效果

可以自行修改Shader 按ctrl+shift+, 强制重编shader 再运行即可看到修改后的结果
