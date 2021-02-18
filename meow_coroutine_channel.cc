#include "meow_coroutine_channel.h"

using meow::coroutine::Channel;

/**
 * Define zend class entry
 */
zend_class_entry meow_coroutine_channel_ce;
zend_class_entry *meow_coroutine_channel_ce_ptr;

static zend_object_handlers meow_coroutine_channel_handlers;

typedef struct {
    Channel *channel;
    zend_object std;
} coroutine_channel_t;

/* 通过 PHP 对象找到 Channel 对象 */
static coroutine_channel_t *meow_coroutine_channel_fetch_obj(zend_object *obj)
{
    return (coroutine_channel_t *)((char *)obj - meow_coroutine_channel_handlers.offset);
}

/* 创建 PHP 对象 */
static zend_object *meow_coroutine_channel_create_obj(zend_class_entry *ce)
{
    coroutine_channel_t *cc = (coroutine_channel_t *) ecalloc(1, sizeof(coroutine_channel_t) + zend_object_properties_size(ce));
    /* 初始化对象 */
    zend_object_std_init(&cc->std, ce);
    /* 初始化对象的属性 */
    object_properties_init(&cc->std, ce);
    cc->std.handlers = &meow_coroutine_channel_handlers;
    return &cc->std;
}

/* 释放 PHP 对象 */
static void meow_coroutine_channel_free_obj(zend_object *obj)
{
    coroutine_channel_t *cc = meow_coroutine_channel_fetch_obj(obj);
    Channel *chan = cc->channel;

    if (chan) {
        /* 释放 Channel 中的数据 */
        while (!chan->empty()) {
            zval *data;
            data = (zval *) chan->pop_data();
            zval_ptr_dtor(data);
            efree(data);
        }

        delete chan;
    }

    zend_object_std_dtor(&cc->std);
}

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
    coroutine_channel_t *cc;
    zend_long capacity = 1;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(capacity)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (capacity <= 0) {
        capacity = 1;
    }

    cc = meow_coroutine_channel_fetch_obj(Z_OBJ_P(getThis()));
    cc->channel = new Channel(capacity);

    /* 将 capacity 保存到 Channel 对象上 */
    zend_update_property_long(meow_coroutine_channel_ce_ptr, getThis(), ZEND_STRL("capacity"), capacity);
}

static PHP_METHOD(meow_coroutine_channel, push)
{
    zval *zdata;
    double timeout = -1;
    coroutine_channel_t *cc;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(zdata)
        Z_PARAM_OPTIONAL
        Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    cc = meow_coroutine_channel_fetch_obj(Z_OBJ_P(getThis()));

    Z_TRY_ADDREF_P(zdata);
    zdata = meow_zval_dup(zdata);

    if (!cc->channel->push(zdata, timeout)) {
        Z_TRY_DELREF_P(zdata);
        efree(zdata);
        RETURN_FALSE
    }

    RETURN_TRUE
}

static PHP_METHOD(meow_coroutine_channel, pop)
{
    zval *zdata;
    double timeout = -1;
    coroutine_channel_t *cc;

    ZEND_PARSE_PARAMETERS_START(0, 1)
            Z_PARAM_OPTIONAL
            Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    cc = meow_coroutine_channel_fetch_obj(Z_OBJ_P(getThis()));
    zdata = (zval *) cc->channel->pop(timeout);

    if (!zdata) {
        RETURN_FALSE
    }

    RETVAL_ZVAL(zdata, 0, 0);
    efree(zdata);
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
    /* 自定义 PHP 对象创建、释放的 handler */
    memcpy(&meow_coroutine_channel_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    MEOW_SET_CLASS_OBJ(meow_coroutine_channel, meow_coroutine_channel_create_obj, meow_coroutine_channel_free_obj, coroutine_channel_t, std);
    /* 声明 Coroutine\Channel 属性 */
    zend_declare_property_long(meow_coroutine_channel_ce_ptr, ZEND_STRL("capacity"), 1, ZEND_ACC_PUBLIC);
}
