# 嵌入式课程设计

基于LPC1768实验开发板，设计并实现一个智能万年历。

## 功能说明

### 主页

当板载开关开启后，默认进入主页，显示当前月的日历。并有3个按钮：

Curr Month：显示当前月的日历。

Last Month：显示上一个月的日历。

Next Month：显示下一个月的日历。

![主页](https://kongdeqi.oss-cn-shanghai.aliyuncs.com/soc/%E4%B8%BB%E9%A1%B5.jpg?x-oss-process=image/resize,h_200)

### 设置日期页

按左右键，可以进行年、月、日的选择；按上下键，可以调整具体的数值。

![设置日期页](https://kongdeqi.oss-cn-shanghai.aliyuncs.com/soc/%E8%AE%BE%E7%BD%AE%E6%97%A5%E6%9C%9F%E9%A1%B5.jpg?x-oss-process=image/resize,h_200)

### 设置时间页

按左右键，可以进行时、分、秒的选择；按上下键，可以调整具体的数值。

![设置时间页](https://kongdeqi.oss-cn-shanghai.aliyuncs.com/soc/%E8%AE%BE%E7%BD%AE%E6%97%B6%E9%97%B4%E9%A1%B5.jpg?x-oss-process=image/resize,h_200)

### 更多页

Night Mode：夜间模式，打开后显示屏从"白底黑字"变为"黑底白字"。

Record Date：重要事项，点击后日历将会在指定日期做一个标记。

Recover Date：恢复到上一个日期设置。若日期不小心设置错误，可以回退。

Factory Reset：时间置为出厂设置，方便出错时一键还原。

![设置](https://kongdeqi.oss-cn-shanghai.aliyuncs.com/soc/%E5%A4%9C%E9%97%B4%E6%A8%A1%E5%BC%8F%E8%AE%BE%E7%BD%AE%E9%A1%B5.jpg?x-oss-process=image/resize,h_200)
![夜间模式](https://kongdeqi.oss-cn-shanghai.aliyuncs.com/soc/%E5%A4%9C%E9%97%B4%E6%A8%A1%E5%BC%8F%E6%95%88%E6%9E%9C%E5%9B%BE.jpg?x-oss-process=image/resize,h_200)

## 设计原理

### 星期计算

基姆拉尔森公式，可以计算某年某月某日是星期几：

W = ( d + 2\*m + 3\*(m+1)/5 + y + y/4 - y/100 + y/400 + 1 ) % 7

在公式中，W表示星期几，y表示年，m表示月，d表示日。

注意，该公式有个与其它公式不同的地方：要把1月和2月看成是上一年的13月和13月，如：2004年1月10日转换成2003年13月10日代入计算。

### 全局变量表

由于程序有多个页面，为使得函数之间能够相互"通信"，需要用到多个全局变量。
| 变量名 | 变量功能 |
| - | - |
| unsigned short TextColor, BackColor | 字体颜色、背景颜色。用于开启夜间模式后，全局调整颜色。 |
| uint32_t SecondInt                  | 秒中断时为1。读取该变量，使每秒在屏幕上更新一次时间。 |
| uint32_t DelayCounter               | 定时器计数值。实现光标精确延时，每300ms闪烁一次。 |
| uint16_t X, Y, XL, YL               | 矩形的坐标、长度、宽度。用于实现矩形按钮的光标效果。 |
| char \*LOCATION                     | 当前所处的位置。用于判定当前处于哪一个按钮位置。 |
| char \*DOCK                         | 当前所处的页面。用于判定当前处于4个页面中的哪一个。 |

### 用户函数表
| 函数名 | 函数功能 |
| - | - |
| int main( void )            | 主函数，进行初始化、秒中断处理、定时器中断处理、按键读取。 |
| void Always( void )         | 显示屏幕常显部分，即标题栏和底栏。 |
| void Home( uint8_t value )  | 输入0，切换到Home页。<br>输入键值，在Home页执行对应按键操作。 |
| void Date( uint8_t value )  | 输入0，切换到设置日期页。<br>输入键值，在设置日期页执行对应按键操作。 |
| void Time( uint8_t value )  | 输入0，切换到设置时间页。<br>输入键值，在设置时间页执行对应按键操作。 |
| void More( uint8_t value )  | 输入0，切换到More页。<br>输入键值，在More页执行对应按键操作。 |
| uint8_t Night( uint8_t on ) | 夜间模式。输入0打开，输入1关闭。<br>打开夜间模式后返回1，关闭夜间模式后返回0。 |
| void Factory( void )        | 恢复出厂设置。时间置为2019年5月31日23时59分50秒，方便出错时一键还原。 |
| void Recover( void )        | 恢复到上一个日期设置。每次设置日期时，上一个日期值储存在E2PROM中。 |
| void Nextday( void )        | 标记明天有重要事项。将明天的日期值储存在E2PROM中，显示日历时将会有标记。 |
| void Clear( void )          | 部分清屏。仅清屏日历部分，以便重新打印。 |
| void LineX( int x, int y, int len, uint16_t color ) | 画横线。输入起始横坐标、纵坐标、长度、颜色。 |
| void LineY( int x, int y, int len, uint16_t color ) | 画竖线。输入起始横坐标、纵坐标、长度、颜色。 |
| void Square( int x, int y, int len, int high, uint16_t color ) | 画矩形。输入起始坐标、长、宽、颜色，用于绘制按钮外框。 |
| uint32_t Mark( uint32_t year, uint32_t month )                 | 输入年、月，比较E2PROM中储存的，检测当月是否有重要的日子。 |
| uint8_t HowMany( uint32_t year, uint32_t mon )                 | 输入年、月，计算该月有几天。 |
| void Calendar( uint32_t year, uint32_t month )                 | 输入年、月，绘制日历。 |

## 流程图
### 主函数
![主函数](https://kongdeqi.oss-cn-shanghai.aliyuncs.com/soc/%E4%B8%BB%E5%87%BD%E6%95%B0.jpg?x-oss-process=image/resize,w_500)

### 设置日期
![设置日期](https://kongdeqi.oss-cn-shanghai.aliyuncs.com/soc/%E8%AE%BE%E7%BD%AE%E6%97%A5%E6%9C%9F.jpg?x-oss-process=image/resize,w_500)

### 设置时间
![设置时间](https://kongdeqi.oss-cn-shanghai.aliyuncs.com/soc/%E8%AE%BE%E7%BD%AE%E6%97%B6%E9%97%B4.jpg?x-oss-process=image/resize,w_500)

### 效果图
![效果图](https://kongdeqi.oss-cn-shanghai.aliyuncs.com/soc/20191219_174444.jpg?x-oss-process=image/resize,w_500)
