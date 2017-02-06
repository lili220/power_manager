#ifndef __POWER_MANAGER_H_
#define __POWER_MANAGER_H_

#include <stdio.h>
#include <glib.h>
#include <gio/gio.h>
#include <string.h>
#include <stdlib.h>
#include <gconf/gconf-client.h>

#define PLAN_NAME_BALANCE		"balance"			//默认平衡模式的名称
#define PLAN_NAME_PERFORMANCE	"performance"		//默认高性能模式的名称
#define PLAN_NAME_SAVER			"saver"				//默认节能模式的名称

#define PLAN_ZH_NAME_BALANCE		"平衡"			//默认平衡模式的名称
#define PLAN_ZH_NAME_PERFORMANCE	"高性能"		//默认高性能模式的名称
#define PLAN_ZH_NAME_SAVER			"节能"				//默认节能模式的名称

#define POWER_ROOT_PATH		"/apps/power-manager" //电源管理模块的根路径

#define POWER_BALANCE_PATH	"/apps/power-manager/balance" //默认平衡模式的根路径
#define POWER_PERFORMANCE_PATH	"/apps/power-manager/performance" //默认高性能模式的根路径
#define POWER_SAVER_PATH	"/apps/power-manager/saver" //默认节能模式的根路径
#define POWER_CUSTOM_PATH	"/apps/power-manager/custom" //默认自定义模式的根路径

#define POWER_CUSTOM_BALANCE_PATH	"/apps/power-manager/custom/balance"//自定义平衡模式的跟路径
#define POWER_CUSTOM_PERFORMANCE_PATH	"/apps/power-manager/custom/performance"//自定义高性能模式的跟路径
#define POWER_CUSTOM_SAVER_PATH	"/apps/power-manager/custom/saver"//自定义节能模式的跟路径

#define CUSTOM_DEFAULT_TEMPLET	"balance" //默认的自定义计划创建模板

#define CUSTOM_PLAN_DEFAULT_NAME "custom_plan" //默认的自定义计划名称
#define CUSTOM_PLAN_DEFAULT_ZH_NAME "自定义电源计划" //默认的自定义计划中文名称

#define SYSTEM_SETTINGS_PATH "org.cinnamon.settings-daemon.plugins.power"

GMainLoop *loop;//测试用主循环，合并代码时要删除

//guint notify_func_id;//添加监控配置项变化的回调函数返回的连接ID

GSettings *system_settings; //用于加载系统电源管理模块的gsettings配置信息


/*使用settings-deamon的枚举类型，合并是删除此枚举，直接使用系统的定义*/
typedef enum
{
	CSD_POWER_ACTION_BLANK,
	CSD_POWER_ACTION_SUSPEND,
	CSD_POWER_ACTION_SHUTDOWN,
	CSD_POWER_ACTION_HIBERNATE,
	CSD_POWER_ACTION_INTERACTIVE,
	CSD_POWER_ACTION_NOTHING
}CsdPowerActionType;

/*KeyType用于标识key_value的类型*/
typedef enum 
{
	STRING_KEY,
	INT_KEY,
	BOOL_KEY,
	ENUM_KEY,
	NULL_KEY
}KeyType;

typedef struct _PlanKey 
{
	KeyType key_type;
	char *key_name;
	char *key_value;
}PlanKey;


/*函数声明*/

/**
 * 将配置文件path路径下的key的值设置为value
 * path：配置文件的路径，我们将在这个路径下对key进行操作
 * key：要设置的key的名称
 * value：要对key进行的设置的值
 * 注意：如果path下存在key，则修改key的值为value,如果path下不存在key，则在path路径下新增key，并且将key的值设置为value
 **/
static void
gconf_client_set_string_key_with_path(GConfClient *client, const gchar *path, const gchar* key, const gchar *value, GError **error);
static void
gconf_client_set_bool_key_with_path(GConfClient *client, const gchar *path, const gchar* key, const gboolean value, GError **error);
static void
gconf_client_set_int_key_with_path(GConfClient *client, const gchar *path, const gchar* key, const gint value, GError **error);

/**
 * 获取配置路径plan_path下的配置项key_name的值
 * 返回值：key_name的取值
 * */
static char *
gconf_client_get_string_key_with_path(GConfClient *client, const char *plan_path, const char *key_name);

/**
 * 获取配置路径plan_path下的配置项key_name的值
 * 返回值：key_name的取值
 * */
static int
gconf_client_get_int_key_with_path(GConfClient *client, const char *plan_path, const char *key_name);

/**
 * 获取配置路径plan_path下的配置项key_name的值
 * 返回值：key_name的取值
 * */
static gboolean
gconf_client_get_bool_key_with_path(GConfClient *client, const char *plan_path, const char *key_name);

/**
 * 创建位于电源配置的根路径下的配置项，这个函数创建的选项主要用于维护电源计划方案的整体配置状态
 * 例如：default-count 用于记录默认名称的自定义电源计划个数;balance-key 用于记录首选项中的平衡模式选项名称;second-key 用于记录首选项中第二条记录的名称
 * */
static void
gconf_client_create_preference_keys(GConfClient *client);

/**
 * 用于程序安装后首次运行时，创建默认的电源管理计划
 * */
static void
gconf_client_create_default_plans(GConfClient *client);

/**
 * 用于监测系统gsettings配置变化并反向同步到gconf配置的回调函数
 * 本函数主要用于将系统gsettings配置反向同步到gconf配置中。
 * 本函数主要用于没有通过电源管理界面修改了gsettings的配置项，此时，应该将修改也同步到gconf的配置中，
 * 目的是保持gconf配置与gsettings配置的一致性
 * */
static void
on_system_settings_changed_cb(GSettings *settings,  const char *key, GConfClient *client);

/**
 * 判断一个字符串key_name是否是电源管理的配置项的名称
 * 返回值：TRUE 是电源管理配置项名称， FALSE 不是电源管理配置项名称
 * */
static gboolean
gconf_string_is_plan_key(const char *key_name);

/**
 * 加载系统的gsettings配置信息，保存到全局变量system_settings, 并添加反向同步回调函数
 * 说明：system_settings主要用于将gconf配置同步到系统配置，同时将gesttings配置反向同步到gconf配置中。
 * */
static void
gconf_client_load_system_settings(GConfClient *client);

/**
 * 为首选项电源计划增加信号同步函数
 * */
static void
gconf_client_add_signals_for_preference_plans(GConfClient *client);

/**
 * 为隐藏和附加电源计划添加信号同步函数
 * */
static void
gconf_client_add_signals_for_extend_plans(GConfClient *client);

/**
 * 初始化，用于初始化配置信息, 包括首次运行创建默认的电源计划
 * */
static void
gconf_client_init(GConfClient *client);

/**
 * 创建平衡模式的配置项
 * */
static void
gconf_client_create_default_balance_plan(GConfClient *client);

/**
 * 创建高性能模式的配置项
 * */
static void
gconf_client_create_default_performance_plan(GConfClient *client);

/**
 * 创建节能模式的配置项
 * */
static void
gconf_client_create_default_saver_plan(GConfClient *client);


/**
 * 读取一个路径下的所有配置项的信息
 * dname：要读取的路径名称，例如：/apps/power-manager/balance
 * 这个函数可用于读取使能的电源计划的配置，然后设置gsettings的相应系统配置
 * */
static void
gconf_client_read_dir_entries(GConfClient *client, const gchar *dname);


/**
 *  将字符串src中的basename拼接到dest字符串的尾部
 *  例如：src="/aaa/enabled" dest="/bbb" 返回值："/bbb/enabled"
 *  */
static char *
string_basename_cat(const char *src, const char *dest);


/**
 * 将配置文件中src路径下的entries完全拷贝到dest路径下
 * */
static void
gconf_client_dir_copy(GConfClient *client , const gchar *src, const gchar *dest);


/**
 * 获取当前以默认名称命名的自定义电源管理计划的个数
 * */
static int
gconf_client_get_default_custom_plans_num(GConfClient *client);

/**
 * 将当前以默认名称命名的自定义电源计划的个数设置为n
 * */
static void
gconf_client_set_default_custom_plans_num(GConfClient *client, int n);


/**
 * 创建自定义电源计划的默认配置存储路径
 * 返回值：新创建的电源计划配置项所在的目录名(eg:custom_plan1)
 * */
static const char *
gconf_client_make_default_custom_plan_path(GConfClient *client);

/**
 * 根据配置项所在的目录名创建默认未命名的自定义电源计划中文名称
 * plan_path: 配置项所在的目录名
 * 返回值：默认的自定义电源计划中文名
 * 例如：plan_path=custom_plan1, 返回值为:自定义电源计划1
 * */
static char *
gconf_client_make_zh_name_with_custom_plan_path(GConfClient *client, const char *plan_path);

/**
 * 将plan_path下的电源计划名设置为plan_name
 * 说明：本函数主要修改个电源计划下的配置项plan-name
 * plan_path：电源计划的配置路径，如"/apps/power-manager/custom/balance/custom_plan1"
 * plan_name:要设置的电源计划名，如"自定义电源计划1"
 * */
static void
gconf_client_set_plan_name(GConfClient *client, const char *plan_path, const char *plan_name);


/**
 * 根据模板的名称，为新创建的自定义电源计划创建配置项
 * templet_plan:拷贝的模板名称(eg:balance、custom、performance)
 * custom_plan:自定义的电源计划名(eg: MyCustomPlan)
 * plan_name:新创建的电源计划的名称
 * 如果目标计划已经存在，则不再拷贝
 * */
static void
gconf_client_create_plan_keys_with_templet(GConfClient *client, const char *templet_plan, const char *plan_path, const char *plan_name);

/**
 * 查找在配置路径plan_path下，是否存在名称叫做plan_name的电源计划
 * 返回值：TRUE 存在， FALSE 不存在
 * 这个函数由gconf_client_custom_plan_exists调用
 * */
static gboolean
gconf_client_plan_name_exists_with_path(GConfClient *client,  const char *plan_path, const char *plan_name);

/**
 * 查找配置路径plan_path下的子目录是否存在名称叫做plan_name的电源计划
 * plan_path:配置路径，一般传值为（POWER_CUSTOM_BALANCE_PATH、POWER_CUSTOM_PERFORMANCE_PATH、POWER_CUSTOM_SAVER_PATH)
 * 返回值：TRUE 存在， FALSE 不存在
 * 说明：这个函数用与确定在自定义电源计划中是否已经存在名称plan_name的电源计划
 * 这个函数由gconf_client_custom_plan_exists调用
 * */
static gboolean
gconf_client_plan_name_exists_under_path(GConfClient *client, const char *plan_path, const char *plan_name);

/**
 * 判断一个是否已经存在自定义电源计划名plan_name
 * 返回值：存在返回TRUE， 不存在返回FALSE
 * */
static gboolean
gconf_client_custom_plan_exists(GConfClient *client, const char *plan_name);

/**
 * 判断是否已经存在自定义电源计划配置路径plan_path
 * plan_path:自定义电源计划配置项所在的目录名(eg:balance、 custom_plan1)
 * 返回值：存在返回TRUE， 不存在返回FALSE
 * */
static gboolean
gconf_client_custom_plan_path_exists(GConfClient *client, const char *plan_path);


/**
 * 根据给定的模板，创建自定义电源管理计划
 * templet_name:是模板的名称
 * plan_name:是自定义计划的名称
 * 如果第2个参数为NULL, 则使用平衡模式为模板创建新的电源计划
 * 如果第3个参数为NULL, 则使用custom_plan[1 2 3....]为自定义电源计划的名称, 后缀的数字自动递增
 * */
void
gconf_client_create_custom_plan(GConfClient *client, const char *templet_name, const char *plan_name);

/**
 * 根据自定义电源计划的名称plan_name获取其配置存储路径
 * 返回值：自定义电源计划plan_name的配置存储路径
 * */
static char *
gconf_client_get_custom_plan_path_with_name(GConfClient *client, const char *plan_path, const char *plan_name);

/**
 * 获取第一条首选项电源计划名称
 * 返回值：第一条首选项电源计划名称(平衡模式或自定义平衡模式)
 * */
char *
gconf_client_get_first_preference_plan_name(GConfClient *client);

/**
 * 获取第二条首选项电源计划名称
 * 返回值：第二条首选项电源计划名称
 * */
char *
gconf_client_get_second_preference_plan_name(GConfClient *client);

/**
 * 获取当前使能的电源计划名称
 * 返回值：使能的电源计划名称
 * */
char *
gconf_client_get_enabled_plan_name(GConfClient *client);

/**
 * 判断一个电源计划是否是首选项中的电源计划
 * 返回值：TRUE 是首选项中的电源计划， FALSE 不是首选项中的电源计划
 * */
static gboolean
gconf_client_is_preference_plan(GConfClient *client, const char *plan_name);

/**
 * 判断一个电源计划是否是附加和隐藏电源计划
 * 返回值：TRUE 是附加和隐藏电源计划， FALSE 不是附加和隐藏电源计划
 * */
static gboolean
gconf_client_is_extend_plan(GConfClient *client, const char *plan_name);

/**
 * 判断一个电源计划是否是正在使能的电源计划
 * 返回值：TRUE 是正在使能的电源计划， FALSE 不是正在使能的电源计划
 * */
static gboolean
gconf_client_is_enabled_plan(GConfClient *client, const char *plan_name);

/**
 * 链表排序用比较函数
 * */
static gint
plan_compare(gconstpointer a, gconstpointer b);

/**
 * 链表排序函数
 * 返回值：排序好的链表
 * 说明：排序原则，非默认名称排在前，默认名称按后缀升序排列
 * */
static GSList *
sort_extend_custom_plans(GSList *list);

/**
 * 获取指定模板的自定义电源计划中的附加和隐藏电源计划名称
 * 返回值：由指定模板的自定义电源计划中的附加电源计划名组成的GSList
 * 本函数由gconf_client_get_extend_plans调用 
 * */
static GSList *
gconf_client_get_extend_custom_plans_with_templet(GConfClient *client, const char *templet_name);

/**
 * 获取所有附加电源计划的名称
 * 返回值：返回所有附加电源计划的名称链表
 * */
GSList *
gconf_client_get_extend_plans(GConfClient *client);

/**
 * 根据key_path获取key的名称
 * 返回值：key的basename, 如果不存在basename（以'/'字符结尾)则返回NULL。
 * 例如：key_path=“/apps/power-manager/custom/saver/custom_plan5/sleep-display-ac”, 返回值为"sleep-display-ac"
 * */
static char *
gconf_string_key_get_basename(const char *key_path);

/**
 * 根据key_name获取key的类型
 * 返回值：KeyTpe
 * */
static KeyType
gconf_string_key_get_type(const char *key_name);

/**
 * 将字符串类型的key_value，转换为enum类型的值
 * 返回值：成功返回CsdPowerActionType, 失败 -1
 * */
static CsdPowerActionType
gconf_string_key_to_action_type(const char *key_value);

/**
 * 将enum类型的动作转换为字符串格式
 * 返回值：字符串格式的动作
 * */
static char *
gconf_enum_action_to_string(CsdPowerActionType action);

/**
 * 将plan_name对应的gconf配置项同步到系统的gsettings下相应的配置项下
 * */
static void
gconf_client_sync_plan_settings_to_system(GConfClient *client, const char *plan_name);

/**
 * 删除当前的配置项同步处理的信号函数
 * */
static void
gconf_client_disable_signals(GConfClient *client);

/**
 * 电源计划配置项同步处理的信号处理函数
 * */
static void
gconf_client_plan_notify_cb(GConfClient *client, guint func_id, GConfEntry *entry, gpointer user_data);

/**
 * 为plan_name对应的gconf配置添加同步的信号处理函数
 * */
static void
gconf_client_add_signals_for_plan(GConfClient *client, const char *plan_name);

/**
 * 监控电源计划plan_name的配置路径下的配置项的变化
 * */
static void
gconf_client_monitor_plan_by_name(GConfClient *client, const char *plan_name);

/**
 * 取消监控电源计划plan_name的配置项的变化情况
 * */
static void
gconf_client_unmonitor_plan_by_name(GConfClient *client, const char *plan_name);

/**
 * 使能plan_name指定的电源计划
 * 说明：如果plan_name已经是使能的电源计划了，则什么都不做。
 * 如果plan_name不是首选计划，则先将plan_name设置为首选计划，再使能plan_name
 * 如果plan_name的模板是平衡模式，则将根路径下的balance-enabled配置项设置为TRUE，否则将balance-enabled设置为FALSE
 * */
void
gconf_client_enable_plan_by_name(GConfClient *client, const char *plan_name);

/**
 * 根据电源计划名称plan_name获取电源计划配置路径
 * 返回值：电源计划plan_name的配置路径
 * */
static char *
gconf_client_get_plan_path_by_name(GConfClient *client, const char *plan_name);


/**
 * 获取plan_name指定的电源计划所使用的模板名称
 * 返回值：plan_name的模板名称
 * */
static char *
gconf_client_get_templet_by_name(GConfClient *client, const char *plan_name);

/**
 * 将plan_name指定的电源计划设置为首选项电源计划
 * 说明：如果plan_name的模板是平衡模式，
 * 则将plan_name设置为首选项的第一条电源计划
 * 否则，将plan_name设置为首选项的第二条电源计划
 * */
static void
gconf_client_set_preference_plan(GConfClient *client, const char *plan_name);

/**
 * 根据电源计划名称，删除一个自定义电源计划
 * */
void
gconf_client_delete_custom_plan_by_name(GConfClient *client, const char *plan_name);

/**
 * 将电源计划plan_name的配置项key的值设置为value
 * plan_name：配置项key所属的电源计划的名称
 * key：要修改的配置项
 * value：key的值字符串
 * 说明：如果value的值不是字符串类型，则需要转换为字符串
 * 例如bool类型值转换为"TRUE"、"FALSE"
 * enum类型，转换为成员名称，例如"suspend"、"hibernate"
 * */
void
gconf_client_set_plan_key_value(GConfClient *client, const char *plan_name, const char *key, const char *value);

/**
 * 设置/修改电源计划plan_name的string类型的配置项key的值为字符串value
 * */
void
gconf_client_set_string_plan_key_value(GConfClient *client, const char *plan_name, const char *key, const char *value);

/**
 * 设置/修改电源计划plan_name的int类型的配置项key的值为整形数值value
 * */
void
gconf_client_set_int_plan_key_value(GConfClient *client, const char *plan_name, const char *key, int value);

/**
 * 设置/修改电源计划plan_name的bool类型的配置项key的值为布尔类型值value
 * */
void
gconf_client_set_bool_plan_key_value(GConfClient *client, const char *plan_name, const char *key, gboolean value);

/**
 * 设置/修改电源计划plan_name的enum类型的配置项key的值为enum类型value
 * 说明：gconf不支持enum类型的配置项，所以是以string类型存储的，提取时会自动从string类型转换为enum类型
 * */
void
gconf_client_set_enum_plan_key_value(GConfClient *client, const char *plan_name, const char *key, int value);

/**
 * 获取电源计划plan_name的配置项key的值, 不需要知道key的类型
 * plan_name：key所在的电源计划的名称
 * key：要获取的配置项的名称
 * 返回值：字符串类型
 * 对于非字符串类型的key值，转化为字符串类型，例如："TRUE"、"FALSE"、"suspend"、"hibernate"
 * */
char *
gconf_client_get_plan_key_value(GConfClient *client, const char *plan_name, const char *key);

/**
 * 获取电源计划plan_name下的string类型的配置项key的值
 * 返回值：key的值
 * */
char *
gconf_client_get_string_plan_key_value(GConfClient *client, const char *plan_name, const char *key);

/**
 * 获取电源计划plan_name下的int类型的配置项key的值
 * 返回值：key的值
 * */
int
gconf_client_get_int_plan_key_value(GConfClient *client, const char *plan_name, const char *key);

/**
 * 获取电源计划plan_name下的bool类型的配置项key的值
 * 返回值：key的值
 * */
gboolean
gconf_client_get_bool_plan_key_value(GConfClient *client, const char *plan_name, const char *key);

/**
 * 获取电源计划plan_name的enum类型的配置项的值
 * 返回值：key的值
 * */
CsdPowerActionType
gconf_client_get_enum_plan_key_value(GConfClient *client, const char *plan_name, const char *key);

/**
 * 判断一个plan_name指定的电源计划是否可以删除
 * 返回值：TRUE 可以删除， FALSE 不可以删除
 * */
gboolean
gconf_client_plan_is_deletable(GConfClient *client, const char *plan_name);
#endif //__POWER_MANAGER_H_
