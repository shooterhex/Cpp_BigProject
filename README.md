# WedDownload

## 项目描述 ##
webDownload项目主要用于方便快捷的下载网页中的html源文件，文本内容和图片。

## 团队描述 ##
小组名称：web内容下载

小组成员：李智，郑无恙，田文杰

选题：简单web内容下载
### 项目管理 ###

版本控制：git, github

开发环境：QT 5.14.2

编程语言：C++17

框架结构：MVVM

文档编写：markdown

### 项目提出 ###


### 功能实现 ###

#### 内容下载

· 输入URL，保存目录

· 选择文件类型

· 下载

#### 下载进度显示

· 显示任务队列

· 在状态栏中显示当前任务相关状态


#### 界面与功能展示

主界面

此处应插入图片

## 具体实现

### 框架描述 ###

框架整体采用MVVM模式，将view层与model层分离，通过view model连接，实现多人协作。

### 实现过程 ###

第一轮迭代：做出主窗口，实现url，目录的输入，点击按钮，获取html内容，保存到内部变量中

第二轮迭代：增加保存文件类型选择（html/文字/图片).
	model层:完成实际文件保存，增加参数type，注意检查参数合法性，如果下载失败请返回false。此轮只实现文本下载。
	window层:传递type，检查comand返回值，若为false，弹出窗口表示下载失败。增加第二个空窗口。要求效果：类似于两个tab页，可以切换
		原窗口标题为 “主页”，新的空窗口标题为“任务列表”
	viewModel:对上下两层提供接口，内部进行必要参数转换。
	type类型设为int，采用宏定义方式对不同类型进行标识。宏定义文件为common/def.h,请自行包含

第三轮迭代：window层实现任务列表显示
	model实现图片下载
	viewModel对上下两层提供接口，内部进行必要参数转换。对window层提供任务列表的样例数据（因为此时model层还没有实现任务列表

第四轮迭代：windows实现底部状态栏，显示当前任务状态；细节优化，任务列表的tab标题：“任务列表（N）”N表示任务数量
	model实现下载线程与主线程分开，删除调试用弹窗
	viewModel维护任务队列，当model中没有任务或结束一个任务时，弹出任务传递给model，同时通知window更新界面

第五轮迭代：增加任务窗口，显示任务列表

第六轮迭代：实现任务排队，多任务下载

## 项目分工 ##

View：田文杰

Model：郑无恙

ViewModel : 李智