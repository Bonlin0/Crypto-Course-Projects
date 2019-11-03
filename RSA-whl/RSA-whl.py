import random


def miller_rabin(n: int, k: int) -> bool:
    # 首先如果是偶数一定是合数
    if n & 1 == 0:
        return False
    # 1和小于1的数都不考虑
    if n<2:
        return False
    # 由于后面的随机数生成中 可能会报错 所以对于小的整数直接判断
    if n <10:
        return n in {2, 3, 5, 7}
    # 计算 n-1 = 2^s * t , t 为奇整数
    t = n - 1
    s = 0
    while not (t & 1):
        s += 1
        t >>= 1
    for i in range(k):
        # 随机选取整数b 2~n-2
        b = random.randint(2, n - 2)
        r0 = pow(b,t,n)
        # 如果满足条件则通过 再取随机数测试
        if r0==1 or r0==n-1:
            continue
        ri = pow(r0,2,n)
        # counter 其实就是教材伪代码的i 但是i在外层循环使用了
        counter = 1
        # if_pass 布尔变量用于标明是否通过，因为两层循环不能直接用continue
        if_pass = 0
        while counter != s:
            if ri==n-1:
                if_pass = 1
                break
            ri = pow(ri,2,n)
            counter += 1
        # 如果i=s-1后还不通过，确认是合数 return false
        if if_pass == 0:
            return False
    # 通过了选择的k个参数还是没有return false
    return True


# 输入位数 输出对应位数的素数
def get_prime(bits: int) -> int:
    while True:
        # 首先获取一个bits位数的int
        a = random.randint(1<<(bits-1),(1<<bits)-1)
        if miller_rabin(a,4):
            return a


# 参考信安数基课本得到的求贝祖等式的递归实现
def calculator_bezout(s0, t0, q, r0, s1, t1, r1): # a > b      s * a + t * b = (a,b)   t is 1 initially while s=0
    s2 = s0 - q * s1
    t2 = t0 - q * t1
    q1 = r0 // r1
    r2 = r0 - q1 * r1
    if r2 == 0:
        st_tuple = (s2, t2)
        return st_tuple
    return calculator_bezout(s1, t1, q1, r1, s2, t2, r2)


# 求模n下x的乘法逆元 只有当n和x 互素时结果正确
def get_inverse(x:int, n:int)->int:
    x = x % n
    # a = n b = x
    (n_inverse, x_inverse) = calculator_bezout(1, 0, n // x, x, 0, 1, n % x)
    if x_inverse<0:
        x_inverse += n
    return x_inverse


# 密钥生成算法
# 输入密钥长度 输出 n, e, p, q, d, phi(n)
def get_keys(bits:int) -> (int, int, int,int, int, int):
    p = get_prime(bits//2)
    q = get_prime(bits//2)
    n = p*q
    e = 65537
    phi_n = (p-1)*(q-1)
    d = get_inverse(e,phi_n)
    return (n, e, p, q, d, phi_n)


# RSA 加密 输入 消息，e，n 输出密文
def RSA_enc(msg:int, e:int, n:int) -> int:
    return pow(msg,e,n)


# RSA 解密 输入 密文，d，n 输出明文
def RSA_dec(cypher:int, d:int, n:int) -> int:
    return pow(cypher,d,n)

# 测试部分
key_set = get_keys(1024)
print(key_set)
c = RSA_enc(12345678910,key_set[1],key_set[0])
print(c)
m = RSA_dec(c,key_set[4],key_set[0])
print(m)