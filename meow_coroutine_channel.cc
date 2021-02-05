#include "meow_coroutine_channel.h"

using meow::coroutine::Channel;

/**
 * Define zend class entry
 */
zend_class_entry meow_coroutine_channel_ce;
zend_class_entry *meow_coroutine_channel_ce_ptr;

/* 定义 Meow\Coroutine\Channel::__construct() 方法的参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_channel_construct, 0, 0, 0)
    ZEND_ARG_INFO(0, capacity)
ZEND_END_ARG_INFO()

/* 定义 Meow\Coroutine\Channel::push() 方法的参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_channel_push, 0, 0, 0)
    ZEND_ARG_INFO(0, data)
    ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

/* 定义 Meow\Coroutine\Channel::pop() 方法的参数 */
ZEND_BEGIN_ARG_INFO_EX(arginfo_meow_coroutine_channel_pop, 0, 0, 0)
    ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

/* 构造函数 */
static PHP_METHOD(meow_coroutine_channel, __construct)
{
    zval zchan;
    zend_long capacity = 1;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(capacity)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (capacity <= 0) {
        capacity = 1;
    }

    Channel *chan = new Channel(capacity);
    ZVAL_PTR(&zchan, chan);

    /* 将 chan 和 capacity 保存到 Channel 对象上 */
    zend_update_property(meow_coroutine_channel_ce_ptr, getThis(), ZEND_STRL("zchan"), &zchan);
    zend_update_property_long(meow_coroutine_channel_ce_ptr, getThis(), ZEND_STRL("capacity"), capacity);
}

static PHP_METHOD(meow_coroutine_channel, push)
{
    zval *zchan;
    Channel *chan;
    zval *zdata;
    double timeout = -1;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(zdata)
        Z_PARAM_OPTIONAL
        Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    zchan = meow_zend_read_property(meow_coroutine_channel_ce_ptr, getThis(), ZEND_STRL("zchan"), 0);
    chan = (Channel *) Z_PTR_P(zchan);

    if (!chan->push(zdata, timeout)) {
        RETURN_FALSE
    }

    RETURN_TRUE
}

static PHP_METHOD(meow_coroutine_channel, pop)
{
    zval *zchan;
    Channel *chan;
    zval *zdata;
    double timeout = -1;

    ZEND_PARSE_PARAMETERS_START(0, 1)
            Z_PARAM_OPTIONAL
            Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    zchan = meow_zend_read_property(meow_coroutine_channel_ce_ptr, getThis(), ZEND_STRL("zchan"), 0);
    chan = (Channel *) Z_PTR_P(zchan);
    zdata = (zval *) chan->pop(timeout);

    if (!zdata) {
        RETURN_FALSE
    }

    RETVAL_ZVAL(zdata, 0, 0);
}

/* Coroutine\Channel 的方法列表 */
static const zend_function_entry meow_coroutine_channel_methods[] =
{
    PHP_ME(meow_coroutine_channel, __construct, arginfo_meow_coroutine_channel_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(meow_coroutine_channel, push, arginfo_meow_coroutine_channel_push, ZEND_ACC_PUBLIC)
    PHP_ME(meow_coroutine_channel, pop, arginfo_meow_coroutine_channel_pop, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

void meow_coroutine_channel_init()
{
    /* 初始化 Coroutine\Channel */
    INIT_NS_CLASS_ENTRY(meow_coroutine_channel_ce, "Meow", "Coroutine\\Channel", meow_coroutine_channel_methods);
    /* 在 Zend 引擎中注册 Coroutine\Channel */
    meow_coroutine_channel_ce_ptr = zend_register_internal_class(&meow_coroutine_channel_ce TSRMLS_CC);
    /* 声明 Coroutine\Channel 属性 */
    zval *zchan = (zval *) malloc(sizeof(zval));
    zend_declare_property(meow_coroutine_channel_ce_ptr, ZEND_STRL("zchan"), zchan, ZEND_ACC_PUBLIC);
    zend_declare_property_long(meow_coroutine_channel_ce_ptr, ZEND_STRL("capacity"), 1, ZEND_ACC_PUBLIC);
}
