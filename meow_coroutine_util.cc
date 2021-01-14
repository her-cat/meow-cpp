#include "unordered_map"
#include "meow_coroutine.h"

using meow::PHPCoroutine;
using meow::Coroutine;

/* 记录用户让出的协程 */
static std::unordered_map<long, Coroutine *> user_yield_coroutines;

/* 定义无参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_study_coroutine_void, 0, 0, 0)
ZEND_END_ARG_INFO()

/* 定义 Coroutine::create 方法的参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_create, 0, 0, 1)
    ZEND_ARG_CALLABLE_INFO(0, func, 0)
ZEND_END_ARG_INFO()

/* 定义 Coroutine::resume 方法的参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_resume, 0, 0, 1)
    ZEND_ARG_INFO(0, cid)
ZEND_END_ARG_INFO()

/* 创建协程 */
PHP_METHOD(meow_coroutine_util, create)
{
    long cid;
    /* 用来存放 create 传递的函数 */
    zend_fcall_info fci = empty_fcall_info;
    zend_fcall_info_cache fcc = empty_fcall_info_cache;

    /* -1 表示没有最大参数个数限制 */
    ZEND_PARSE_PARAMETERS_START(1, -1)
        Z_PARAM_FUNC(fci, fcc)
        /* 解析可变参数 */
        Z_PARAM_VARIADIC("*", fci.params, fci.param_count)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    cid = PHPCoroutine::create(&fcc, fci.param_count, fci.params);
    RETURN_LONG(cid)
}

/* 让出协程 */
PHP_METHOD(meow_coroutine_util, yield)
{
    Coroutine *coroutine = Coroutine::get_current();
    user_yield_coroutines[coroutine->get_cid()] = coroutine;
    coroutine->yield();
    RETURN_TRUE
}

/* 恢复协程 */
PHP_METHOD(meow_coroutine_util, resume)
{
    zend_long cid;
    Coroutine *coroutine;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(cid)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    auto coroutine_iterator = user_yield_coroutines.find(cid);

    coroutine = coroutine_iterator->second;
    /* 从 user_yield_coroutines 中删除被恢复的协程 */
    user_yield_coroutines.erase(cid);
    /* 恢复协程 */
    coroutine->resume();

    RETURN_TRUE
}

/* 获取当前协程的 id */
PHP_METHOD(meow_coroutine_util, getCid)
{
    RETURN_LONG(Coroutine::get_current()->get_cid())
}

/* 定义 Coroutine 的方法列表 */
const zend_function_entry meow_coroutine_util_methods[] =
{
    PHP_ME(meow_coroutine_util, create, arginfo_meow_coroutine_create, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(meow_coroutine_util, yield, arginfo_study_coroutine_void, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(meow_coroutine_util, resume, arginfo_meow_coroutine_resume, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(meow_coroutine_util, getCid, arginfo_study_coroutine_void, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

zend_class_entry meow_coroutine_ce;
zend_class_entry *meow_coroutine_ce_ptr;

void meow_coroutine_util_init()
{
    /* 初始化 Coroutine */
    INIT_NS_CLASS_ENTRY(meow_coroutine_ce, "Meow", "Coroutine", meow_coroutine_util_methods);
    /* 在 Zend 引擎中注册 Coroutine */
    meow_coroutine_ce_ptr = zend_register_internal_class(&meow_coroutine_ce TSRMLS_CC);
}
