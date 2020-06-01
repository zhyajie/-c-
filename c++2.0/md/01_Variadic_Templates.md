## Variadic Templates ##

数量不定的模板参数：

![](https://i.imgur.com/PheGO3f.png)

下面这个例子演示了如何使用该特性：

![](https://i.imgur.com/R5tYxHV.png)

以print函数为例，该模板参数将可以把参数不断地递归分解参数，上面例子不断递归调用print本身将参数打印出来了，但是最后一个打印完后没有参数了，所以还需要写一个参数为零的print()。 

下面是递归调用的又一个案例，也是关于上图问题的讨论：

![](https://i.imgur.com/EPmMEZa.png)

首先，CustomerHash类中调用hash_val函数，该函数有三个参数，然而hash_val函数一共有四个函数可供选择，这里将选择第一个标记的函数，其他三个函数第一个参数都明确指出要是size_t类型的，只有i地一个函数参数是可变参数。然后第一个参数内部又调用hash_val函数，这次第一个参数是size_t的seed，这里将匹配第二个函数，此时的val是c.fname，args是c.lname和c.no，然后继续调用hash_val函数，这时已经有三个参数了，匹配本身，继续调用第二个函数，然后依次类推，按照上图箭头的调用顺序来完成参数的传递和调用。

下面这个案例更是花哨，为了终止参数：

![](https://i.imgur.com/HlRhcCL.png)

