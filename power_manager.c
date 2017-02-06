#include "power_manager.h"

/*平衡模式下的key列表*/
PlanKey balance_keys[] = 
{
	/*电源计划名称*/
	{STRING_KEY, "plan-name", "平衡"},

	/*关闭显示器*/
	{INT_KEY, "sleep-display-ac", "600"},
	{INT_KEY, "sleep-display-battery", "600"},

	/*唤醒时是否需要密码*/
	{BOOL_KEY, "lock-on-suspend", "FALSE"},

	/*硬盘*/

	/*无线适配器配置*/

	/*睡眠*/
	{ENUM_KEY, "sleep-inactive-ac-type", "suspend"},
	{ENUM_KEY, "sleep-inactive-battery-type", "suspend"},
	{INT_KEY, "sleep-inactive-ac-timeout", "0"},
	{INT_KEY, "sleep-inactive-battery-timeout", "0"},

	/*USB设置*/
	/*电源按钮*/
	{ENUM_KEY, "button-power", "interactive"},

#if 0
	/*睡眠按钮*/
	//{ENUM_KEY, "button-sleep", "hibernate"},
#endif

	/*结束标志*/
	{NULL_KEY, NULL, NULL}
};

/*高性能模式下的key列表*/
PlanKey performance_keys[] = 
{
	/*电源计划名称*/
	{STRING_KEY, "plan-name", "高性能"},

	/*关闭显示器*/
	{INT_KEY, "sleep-display-ac", "0"},
	{INT_KEY, "sleep-display-battery", "0"},

	/*唤醒时是否需要密码*/
	{BOOL_KEY, "lock-on-suspend", "FALSE"},

	/*硬盘*/

	/*无线适配器配置*/

	/*睡眠*/
	{ENUM_KEY, "sleep-inactive-ac-type", "suspend"},
	{ENUM_KEY, "sleep-inactive-battery-type", "suspend"},
	{INT_KEY, "sleep-inactive-ac-timeout", "0"},
	{INT_KEY, "sleep-inactive-battery-timeout", "0"},

	/*USB设置*/
	/*电源按钮*/
	{ENUM_KEY, "button-power", "interactive"},

#if 0
	/*睡眠按钮*/
	{ENUM_KEY, "button-sleep", "hibernate"},
#endif

	/*结束标志*/
	{NULL_KEY, NULL, NULL}
};

/*节能模式下的key列表*/
PlanKey saver_keys[] = 
{
	/*电源计划名称*/
	{STRING_KEY, "plan-name", "节能"},

	/*关闭显示器*/
	{INT_KEY, "sleep-display-ac", "300"},
	{INT_KEY, "sleep-display-battery", "300"},

	/*唤醒时是否需要密码*/
	{BOOL_KEY, "lock-on-suspend", "FALSE"},

	/*硬盘*/

	/*无线适配器配置*/

	/*睡眠*/
	{ENUM_KEY, "sleep-inactive-ac-type", "suspend"},
	{ENUM_KEY, "sleep-inactive-battery-type", "suspend"},
	{INT_KEY, "sleep-inactive-ac-timeout", "0"},
	{INT_KEY, "sleep-inactive-battery-timeout", "0"},

	/*USB设置*/

	/*电源按钮*/
	{ENUM_KEY, "button-power", "interactive"},

#if 0
	/*睡眠按钮*/
	{ENUM_KEY, "button-sleep", "hibernate"},
#endif

	/*结束标志*/
	{NULL_KEY, NULL, NULL}
};


/**
 * client: gconf的客户端
 * path: 要添加的key所在的路径
 * key：要添加的key的名字
 * value：要添加的key的值
 * error：出错信息
 * */
static void
gconf_client_set_string_key_with_path(GConfClient *client, const gchar *path, const gchar* key, const gchar *value, GError **error)
{
	gchar key_buf[128] = {0};
	sprintf(key_buf, "%s/%s", path, key);

	if(error != NULL)
		gconf_client_set_string(client, key_buf, value, error);
	else
		gconf_client_set_string(client, key_buf, value, NULL);

	return ;
}

/**
 * client: gconf的客户端
 * path: 要添加的key所在的路径
 * key：要添加的key的名字
 * value：要添加的key的值
 * error：出错信息
 * */
static void
gconf_client_set_bool_key_with_path(GConfClient *client, const gchar *path, const gchar* key, const gboolean value, GError **error)
{
	gchar key_buf[128] = {0};
	sprintf(key_buf, "%s/%s", path, key);

	if(error != NULL)
		gconf_client_set_bool(client, key_buf, value, error);
	else
		gconf_client_set_bool(client, key_buf, value, NULL);

	return ;
}

/**
 * client: gconf的客户端
 * path: 要添加的key所在的路径
 * key：要添加的key的名字
 * value：要添加的key的值
 * error：出错信息
 * */
static void
gconf_client_set_int_key_with_path(GConfClient *client, const gchar *path, const gchar* key, const gint value, GError **error)
{
	gchar key_buf[128] = {0};
	sprintf(key_buf, "%s/%s", path, key);

	if(error != NULL)
		gconf_client_set_int(client, key_buf, value, error);
	else
		gconf_client_set_int(client, key_buf, value, NULL);

	return ;
}


/**
 * 获取配置路径plan_path下的配置项key_name的值
 * 返回值：key_name的取值
 * */
static char *
gconf_client_get_string_key_with_path(GConfClient *client, const char *plan_path, const char *key_name)
{
	char buf[256] = {0};
	sprintf(buf, "%s/%s", plan_path, key_name);

	return gconf_client_get_string(client, buf, NULL);
}

/**
 * 获取配置路径plan_path下的配置项key_name的值
 * 返回值：key_name的取值
 * */
static int
gconf_client_get_int_key_with_path(GConfClient *client, const char *plan_path, const char *key_name)
{
	char buf[256] = {0};
	sprintf(buf, "%s/%s", plan_path, key_name);

	return gconf_client_get_int(client, buf, NULL);
}

/**
 * 获取配置路径plan_path下的配置项key_name的值
 * 返回值：key_name的取值
 * */
static gboolean
gconf_client_get_bool_key_with_path(GConfClient *client, const char *plan_path, const char *key_name)
{
	char buf[256] = {0};
	sprintf(buf, "%s/%s", plan_path, key_name);

	return gconf_client_get_bool(client, buf, NULL);
}

/**
 * 用于第一次运行时，由于不存在schema文件，要初始化默认的首选项。
 * */
static void
gconf_client_create_preference_keys(GConfClient *client)
{
	GError *error = NULL;
	
	if(gconf_client_dir_exists(client, POWER_ROOT_PATH, &error))
	{
		/* 如果POWER_ROOT_PATH已经存在，证明非第一次运行，首选项的key已经创建完毕，直接返回*/
		printf("%s exist already, now return\n", POWER_ROOT_PATH);
		return;
	}

	/**
	 * 默认首选项显示平衡模式和高性能模式,默认使能的模式是平衡模式
	 * */
	gconf_client_set_int_key_with_path(client, POWER_ROOT_PATH, "default-count", 0, &error);//用于记录默认自定义模式的个数
	gconf_client_set_string_key_with_path(client, POWER_ROOT_PATH, "balance-key", "平衡", &error);
	gconf_client_set_string_key_with_path(client, POWER_ROOT_PATH, "second-key", "高性能", NULL);
	gconf_client_set_bool_key_with_path(client,	POWER_ROOT_PATH, "balance-enabled", TRUE, &error);

	return;
}

/**
 * 创建默认的平衡模式
 * */
static void
gconf_client_create_default_balance_plan(GConfClient *client)
{
	GError *error = NULL;

	if(gconf_client_dir_exists(client, POWER_BALANCE_PATH, &error))
	{
		/*如果默认模式的路径已经存在，则直接返回*/
		printf("%s exists already, now return\n", POWER_BALANCE_PATH);
		return;
	}

	int i;
	for(i = 0; balance_keys[i].key_type != NULL_KEY; i++)
	{
		PlanKey *key = &balance_keys[i];
		switch(key->key_type)
		{
			case STRING_KEY:
				gconf_client_set_string_key_with_path(client, POWER_BALANCE_PATH, key->key_name, key->key_value, &error);
				break;
			case INT_KEY:
				gconf_client_set_int_key_with_path(client, POWER_BALANCE_PATH, key->key_name, atoi(key->key_value), &error);
				break;
			case BOOL_KEY:
				gconf_client_set_bool_key_with_path(client, POWER_BALANCE_PATH, key->key_name, strcmp(key->key_value, "TRUE") == 0 ? TRUE : FALSE, &error);
				break;
			case ENUM_KEY:
				/*对于枚举类型处理要注意，向gconf保存配置时使用string，但是在同步到gsettings时与string类型不同*/
				gconf_client_set_string_key_with_path(client, POWER_BALANCE_PATH, key->key_name, key->key_value, &error);
				break;
			default:
				break;
		}
	}

	return ;
}

/**
 * 创建默认的高性能模式
 * */
static void
gconf_client_create_default_performance_plan(GConfClient *client)
{
	GError *error = NULL;

	if(gconf_client_dir_exists(client, POWER_PERFORMANCE_PATH, &error))
	{
		/*如果默认高性能模式的路径已经存在，则直接返回*/
		printf("%s exists already, now return\n", POWER_PERFORMANCE_PATH);
		return;
	}

	int i;
	for(i = 0; performance_keys[i].key_type != NULL_KEY; i++)
	{
		PlanKey *key = &performance_keys[i];
		switch(key->key_type)
		{
			case STRING_KEY:
				gconf_client_set_string_key_with_path(client, POWER_PERFORMANCE_PATH, key->key_name, key->key_value, &error);
				break;
			case INT_KEY:
				gconf_client_set_int_key_with_path(client, POWER_PERFORMANCE_PATH, key->key_name, atoi(key->key_value), &error);
				break;
			case BOOL_KEY:
				gconf_client_set_bool_key_with_path(client, POWER_PERFORMANCE_PATH, key->key_name, strcmp(key->key_value, "TRUE") == 0 ? TRUE : FALSE, &error);
				break;
			case ENUM_KEY:
				gconf_client_set_string_key_with_path(client, POWER_PERFORMANCE_PATH, key->key_name, key->key_value, &error);
				break;
			default:
				break;
		}
	}

	return ;
}

/**
 * 创建默认的节能模式
 * */
static void
gconf_client_create_default_saver_plan(GConfClient *client)
{
	GError *error = NULL;

	if(gconf_client_dir_exists(client, POWER_SAVER_PATH, &error))
	{
		/*如果默认节能模式的路径已经存在，则直接返回*/
		printf("%s exists already, now return\n", POWER_SAVER_PATH);
		return;
	}

	int i;
	for(i = 0; saver_keys[i].key_type != NULL_KEY; i++)
	{
		PlanKey *key = &saver_keys[i];
		switch(key->key_type)
		{
			case STRING_KEY:
				gconf_client_set_string_key_with_path(client, POWER_SAVER_PATH, key->key_name, key->key_value, &error);
				break;
			case INT_KEY:
				gconf_client_set_int_key_with_path(client, POWER_SAVER_PATH, key->key_name, atoi(key->key_value), &error);
				break;
			case BOOL_KEY:
				gconf_client_set_bool_key_with_path(client, POWER_SAVER_PATH, key->key_name, strcmp(key->key_value, "TRUE") == 0 ? TRUE : FALSE, &error);
				break;
			case ENUM_KEY:
				gconf_client_set_string_key_with_path(client, POWER_SAVER_PATH, key->key_name, key->key_value, &error);
				break;
			default:
				break;
		}
	}

	return ;
}


/**
 * 用于程序安装后首次运行时，创建默认的电源管理计划
 * */
static void
gconf_client_create_default_plans(GConfClient *client)
{
	/*创建默认的平衡模式*/
	gconf_client_create_default_balance_plan(client);

	/*创建默认的高性能模式*/
	gconf_client_create_default_performance_plan(client);

	/*创建默认的节能模式*/
	gconf_client_create_default_saver_plan(client);

	return ;
}

/**
 * 判断一个字符串key_name是否是电源管理的配置项的名称
 * 返回值：TRUE 是电源管理配置项名称， FALSE 不是电源管理配置项名称
 * */
static gboolean
gconf_string_is_plan_key(const char *key_name)
{
	int i;
	for(i = 0; balance_keys[i].key_type != NULL_KEY; i++)
	{
		if(strcmp(balance_keys[i].key_name, key_name) == 0)
			return TRUE;
	}

	return FALSE;
}

/**
 * gsettings反向同步gconf配置的回调函数
 * 本函数主要用于没有通过电源管理界面修改了gsettings的配置项，此时，应该将修改也同步到gconf的配置中，
 * 目的是保持gconf配置与gsettings配置的一致性
 * */
static void
on_system_settings_changed_cb(GSettings *settings,  const char *key, GConfClient *client)
{
	/*如果key不是电源管理配置项的名字，直接返回*/
	if(!gconf_string_is_plan_key(key))
		return ;

	printf("%s :gsettings --> %s changed ......\n", __func__, key);

	/*获取当前使能的电源计划名称*/
	char *plan_name = gconf_client_get_enabled_plan_name(client);

	/*获取发生变动的配置项的类型,根据配置项的不同类型做不同的处理*/
	KeyType key_type = gconf_string_key_get_type(key);
	switch(key_type)
	{
		case STRING_KEY:
			{
				/*string 类型配置项，如果该配置项在gsettings中的值与在gconf配置的值不同，则将gsettings的值反向同步到gconf的配置项下*/
				char *system_key_value = g_settings_get_string(settings, key);
				char *gconf_key_value = gconf_client_get_string_plan_key_value(client, plan_name, key);
				if(strcmp(system_key_value, gconf_key_value) != 0)
				{
					gconf_client_set_plan_key_value(client, plan_name, key, system_key_value);
				}

				free(system_key_value);
				free(gconf_key_value);
			}
			break;
		case ENUM_KEY:
			{
				/*enum 类型配置项，如果该配置项在gsettings中的值与在gconf配置的值不同，则将gsettings的值反向同步到gconf的配置项下*/
				CsdPowerActionType system_key_value = g_settings_get_enum(settings, key);
				CsdPowerActionType gconf_key_value = gconf_client_get_enum_plan_key_value(client, plan_name, key);
				if(system_key_value != gconf_key_value)
				{
					char *enum_str = gconf_enum_action_to_string(system_key_value);
					gconf_client_set_plan_key_value(client, plan_name, key, enum_str);
					
					free(enum_str);
				}
			}
			break;
		case INT_KEY:
			{
				/*int 类型配置项，如果该配置项在gsettings中的值与在gconf配置的值不同，则将gsettings的值反向同步到gconf的配置项下*/
				int system_key_value = g_settings_get_int(settings, key);
				int gconf_key_value = gconf_client_get_int_plan_key_value(client, plan_name, key);
				if(system_key_value != gconf_key_value)
				{
					char buf[32] = {0};
					sprintf(buf, "%d", system_key_value);
					gconf_client_set_plan_key_value(client, plan_name, key, buf);
				}
			}
			break;
		case BOOL_KEY:
			{
				/*bool 类型配置项，如果该配置项在gsettings中的值与在gconf配置的值不同，则将gsettings的值反向同步到gconf的配置项下*/
				gboolean system_key_value = g_settings_get_boolean(settings, key);
				gboolean gconf_key_value = gconf_client_get_bool_plan_key_value(client, plan_name, key);
				if(system_key_value != gconf_key_value)
				{
					char buf[32] = {0};
					sprintf(buf, "%s", system_key_value == TRUE ? "TRUE": "FALSE");
					gconf_client_set_plan_key_value(client, plan_name, key, buf);
				}
			}
			break;
		default:
			break;
	}
}

/**
 * 加载系统的gsettings配置信息，保存到全局变量system_settings, 并添加反向同步回调函数
 * */
static void
gconf_client_load_system_settings(GConfClient *client)
{
	system_settings = g_settings_new(SYSTEM_SETTINGS_PATH);
	g_signal_connect(system_settings, "changed", G_CALLBACK(on_system_settings_changed_cb), client);
}


/**
 * 为首选项电源计划增加信号同步函数
 * */
static void
gconf_client_add_signals_for_preference_plans(GConfClient *client)
{
	char *plan_name = gconf_client_get_first_preference_plan_name(client);
	gconf_client_add_signals_for_plan(client, plan_name);
	free(plan_name);

	plan_name = gconf_client_get_second_preference_plan_name(client);
	gconf_client_add_signals_for_plan(client, plan_name);
	free(plan_name);

	return;
}

/**
 * 为隐藏和附加电源计划添加信号同步函数
 * */
static void
gconf_client_add_signals_for_extend_plans(GConfClient *client)
{
	GSList *plan_names = gconf_client_get_extend_plans(client);

	GSList *tmp = plan_names;
	while(tmp != NULL)
	{
		gconf_client_add_signals_for_plan(client, tmp->data);

		tmp = tmp->next;
	}

	return ;
}

/**
 * 初始化，用于初始化配置信息, 包括首次运行创建默认的电源计划
 * */
static void
gconf_client_init(GConfClient *client)
{
	/*初始化首选项的配置信息*/
	gconf_client_create_preference_keys(client);

	/*创建默认电源管理计划*/
	gconf_client_create_default_plans(client);

	/*为首选项计划添加信号同步函数*/
	gconf_client_add_signals_for_preference_plans(client);

	/*为隐藏和附加计划添加信号同步函数*/
	gconf_client_add_signals_for_extend_plans(client);

	/*加载系统的gsettings配置信息*/
	gconf_client_load_system_settings(client);

	return ;
}

/**
 * 读取一个目录下所有key
 * dname: 要读取目录的绝对路径
 * */
static void
gconf_client_read_dir_entries(GConfClient *client, const gchar *dname)
{
	GError *error = NULL;

	/*获取目录下的所有entries*/
	GSList *entries = gconf_client_all_entries(client, dname, &error);

	/*遍历所有entries，获取每个entry下的key和value*/
	GSList *tmp = entries;
	while(tmp != NULL)
	{
		gconf_entry_set_is_default(tmp->data, 1);
		const char *key = gconf_entry_get_key(tmp->data);	
		GConfValue *value = gconf_entry_get_value(tmp->data);
		switch(value->type)
		{
			case GCONF_VALUE_STRING:
				{
					const char *value_str = gconf_value_get_string(value);
					printf("%s == %s\n", key, value_str);
				}
				break;
			case GCONF_VALUE_INT:
				{
					int value_int = gconf_value_get_int(value);
					printf("%s == %d\n", key, value_int);
				}
				break;
			case GCONF_VALUE_BOOL:
				{
					gboolean value_bool = gconf_value_get_bool(value);
					printf("%s == %d\n", key, value_bool);
				}
				break;
			case GCONF_VALUE_PAIR:
				break;
			case GCONF_VALUE_LIST:
				break;
			default:
				break;

		}
		tmp = tmp->next;
	}

	g_slist_free(entries);

	return ;
}

/**
 * 用于读取配置文件的信息
 * */
static void
read_conf(GConfClient *client)
{
	GError *error = NULL;

	/*处理电源管理根路径下的entry，即首选项的配置信息*/
	gconf_client_read_dir_entries(client, POWER_ROOT_PATH);

	/*获取所有子目录*/
	GSList *dirs = gconf_client_all_dirs(client, POWER_ROOT_PATH, &error);

	/*遍历所有子目录*/
	printf("subdirs of %s:\n", POWER_ROOT_PATH);
	GSList *tmp = dirs;
	while(tmp != NULL)
	{
		printf("%s\n", (char*)tmp->data);
		gconf_client_read_dir_entries(client, tmp->data);
		tmp = tmp->next;
	}

	g_slist_free(dirs);

	return ;
}

/**
 * 将字符串src中的basename拼接到dest字符串的尾部
 * 例如：src="/aaa/enabled" dest="/bbb" 返回值："/bbb/enabled"
 * */
static char *
string_basename_cat(const char *src, const char *dest)
{
	const char *p = strrchr(src, '/');
	int len = strlen(dest) + strlen(p);

	char *str = strdup(dest);
	str = (char *)realloc(str, len + 1);
	strcat(str, p);

	return str;
}

/**
 * 将src路径下的entries完全拷贝到dest路径下
 * */
static void
gconf_client_dir_copy(GConfClient *client , const gchar *src, const gchar *dest)
{
	GError *error = NULL;

	/*获取源目录下的entries*/
	GSList *entries = gconf_client_all_entries(client, src, &error);

	GSList *tmp = entries;
	while(tmp != NULL)
	{
		const char *key_str = gconf_entry_get_key(tmp->data);
		const char *key_dest = string_basename_cat(key_str, dest);
		printf("key_dest == %s\n", key_dest);
		/*拷贝entry， 然后修改entry的key, 添加entry*/
		GConfValue *value = gconf_entry_get_value(tmp->data);
		switch(value->type)
		{
			case GCONF_VALUE_STRING:
				{
					const char *value_str = gconf_value_get_string(value);
					gconf_client_set_string(client, key_dest, value_str, &error);
				}
				break;
			case GCONF_VALUE_INT:
				{
					int value_int = gconf_value_get_int(value);
					gconf_client_set_int(client, key_dest, value_int, &error);
				}
				break;
			case GCONF_VALUE_BOOL:
				{
					gboolean value_bool = gconf_value_get_bool(value);
					gconf_client_set_bool(client, key_dest, value_bool, &error);
				}
				break;
			case GCONF_VALUE_PAIR:
				break;
			case GCONF_VALUE_LIST:
				break;
			default:
				break;

		}

		tmp = tmp->next;
	}

	g_slist_free(entries);

	return ;
}

/**
 * 获取当前的默认自定义电源计划的个数
 * */
static int
gconf_client_get_default_custom_plans_num(GConfClient *client)
{
	char buf[256] = {0};
	sprintf(buf, "%s/default-count", POWER_ROOT_PATH);
	int num = gconf_client_get_int(client, buf, NULL);

	return num;
}

/**
 * 设置当前的默认自定义电源计划的个数为n
 * */
static void
gconf_client_set_default_custom_plans_num(GConfClient *client, int n)
{
	char buf[256] = {0};
	sprintf(buf, "%s/default-count", POWER_ROOT_PATH);
	gconf_client_set_int(client, buf, n, NULL);

	return ;
}

/**
 * 判断一个是否已经存在自定义电源计划配置路径plan_path
 * 返回值：存在返回TRUE， 不存在返回FALSE
 * */
static gboolean
gconf_client_custom_plan_path_exists(GConfClient *client, const char *plan_path)
{
	char buf[256] = {0};
	/*如果自定义的平衡模式下存在此名称的自定义计划，返回TRUE*/
	sprintf(buf, "%s/%s", POWER_CUSTOM_BALANCE_PATH, plan_path);
	if(gconf_client_dir_exists(client, buf, NULL))
		return TRUE;

	/*如果自定义的高性能模式下存在此名称的自定义计划，返回TRUE*/
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%s/%s", POWER_CUSTOM_PERFORMANCE_PATH, plan_path);
	if(gconf_client_dir_exists(client, buf, NULL))
		return TRUE;

	/*如果自定义的节能模式下存在此名称的自定义计划，返回TRUE*/
	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%s/%s", POWER_CUSTOM_SAVER_PATH, plan_path);
	if(gconf_client_dir_exists(client, buf, NULL))
		return TRUE;

	/*不存在这个名称的自定义电源计划，返回FALSE*/
	return FALSE;
}

/**
 * 获取第一条首选项电源计划名称
 * 返回值：第一条首选项电源计划名称(平衡模式或自定义平衡模式)
 * */
char *
gconf_client_get_first_preference_plan_name(GConfClient *client)
{
	/*第一条首选项电源计划的名称存储在电源跟路径下的balance-key配置项下*/
	char buf[256] = {0};
	sprintf(buf, "%s/%s", POWER_ROOT_PATH, "balance-key");
	char *str = gconf_client_get_string(client, buf, NULL);

	return str;
}

/**
 * 获取第二条首选项电源计划名称
 * 返回值：第二条首选项电源计划名称
 * */
char *
gconf_client_get_second_preference_plan_name(GConfClient *client)
{
	/*第二条首选项电源计划的名称存储在电源跟路径下的second-key配置项下*/
	char buf[256] = {0};
	sprintf(buf, "%s/%s", POWER_ROOT_PATH, "second-key");
	char *str = gconf_client_get_string(client, buf, NULL);

	return str;
}

/**
 * 获取当前使能的电源计划名称
 * 返回值：使能的电源计划名称
 * */
char *
gconf_client_get_enabled_plan_name(GConfClient *client)
{
	char buf[256] = {0};
	char *str = NULL;

	/*如果平衡模式使能，则返回第一条首选项名称，否则返回第二条首选项名称*/
	sprintf(buf, "%s/%s", POWER_ROOT_PATH, "balance-enabled");
	gboolean balance_enabled = gconf_client_get_bool(client, buf, NULL);

	if(balance_enabled)
		str = gconf_client_get_first_preference_plan_name(client);
	else
		str = gconf_client_get_second_preference_plan_name(client);

	return str;
}

/**
 * 根据配置项所在的目录名创建默认未命名的自定义电源计划中文名称
 * plan_path: 配置项所在的目录名
 * 返回值：默认的自定义电源计划中文名
 * 例如：plan_path=custom_plan1, 返回值为:自定义电源计划1
 * */
static char *
gconf_client_make_zh_name_with_custom_plan_path(GConfClient *client, const char *plan_path)
{
	char buf[256] = {0};

	/*中文自定义电源计划名称由两部分组成：CUSTOM_PLAN_DEFAULT_ZH_NAME宏后接一个整形数字*/
	const char *p = strstr(plan_path, CUSTOM_PLAN_DEFAULT_NAME);
	p += strlen(CUSTOM_PLAN_DEFAULT_NAME);

	sprintf(buf, "%s%s", CUSTOM_PLAN_DEFAULT_ZH_NAME, p);

	return strdup(buf);
}

/**
 * 将plan_path下的电源计划名设置为plan_name
 * 说明：本函数主要修改个电源计划下的配置项plan-name
 * plan_path：电源计划的配置路径，如"/apps/power-manager/custom/balance/custom_plan1"
 * plan_name:要设置的电源计划名，如"自定义电源计划1"
 * */
static void
gconf_client_set_plan_name(GConfClient *client, const char *plan_path, const char *plan_name)
{
	gconf_client_set_string_key_with_path(client, plan_path, "plan-name", plan_name, NULL);

	return ;
}

/**
 * 测试用函数，读取打印隐藏计划名称
 * */
static void
gconf_client_read_extend_plans(GConfClient *client, GSList *plans)
{
	printf("read_extend_plans\n");
	GSList *tmp = plans;
	while(tmp != NULL)
	{
		printf("%s\n", (char *)tmp->data);
		tmp = tmp->next;
	}

	return;
}

/**
 * 判断一个电源计划是否是首选项中的电源计划
 * 返回值：TRUE 是首选项中的电源计划， FALSE 不是首选项中的电源计划
 * */
static gboolean
gconf_client_is_preference_plan(GConfClient *client, const char *plan_name)
{
	gboolean ret = FALSE;

	/*获取两个首选项的电源计划名称*/
	char *first_plan = gconf_client_get_first_preference_plan_name(client);
	char *second_plan = gconf_client_get_second_preference_plan_name(client);

	/*如果plan_name与两个首选计划中的一个相同，则证明plan_name是首选计划*/
	if((strcmp(plan_name, first_plan) == 0) || (strcmp(plan_name, second_plan) == 0))
		ret = TRUE;

	free(first_plan);
	free(second_plan);

	/*plan_name与两个首选计划都不相同，证明plan_name不是首选计划*/
	return ret;
}

/**
 * 判断一个电源计划是否是附加和隐藏电源计划
 * 返回值：TRUE 是附加和隐藏电源计划， FALSE 不是附加和隐藏电源计划
 * */
static gboolean
gconf_client_is_extend_plan(GConfClient *client, const char *plan_name)
{
	gboolean ret = FALSE;

	/*获取两个首选项的电源计划名称*/
	char *first_plan = gconf_client_get_first_preference_plan_name(client);
	char *second_plan = gconf_client_get_second_preference_plan_name(client);

	/*如果plan_name与两个首选计划都不相同，则证明plan_name是附加和隐藏电源计划*/
	if((strcmp(plan_name, first_plan) != 0) && (strcmp(plan_name, second_plan) != 0))
		ret = TRUE;

	free(first_plan);
	free(second_plan);

	/*plan_name不是附加和隐藏电源计划*/
	return ret;
}

/**
 * 判断一个电源计划是否是正在使能的电源计划
 * 返回值：TRUE 是正在使能的电源计划， FALSE 不是正在使能的电源计划
 * */
static gboolean
gconf_client_is_enabled_plan(GConfClient *client, const char *plan_name)
{
	gboolean ret = FALSE;

	/*如果plan_name与正在使能的电源计划名称相同，则plan_name是使能的电源计划*/
	char *enabled_plan = gconf_client_get_enabled_plan_name(client);
	printf("enabled plan name is %s\n", enabled_plan);
	if(strcmp(plan_name, enabled_plan) == 0)
		ret = TRUE;

	free(enabled_plan);

	return ret;
}

/**
 * 链表排序用比较函数
 * */
static gint
plan_compare(gconstpointer a, gconstpointer b)
{
	/*如果第一个参数是非默认名称，则第一个在前面*/
	if(strstr(a, CUSTOM_PLAN_DEFAULT_ZH_NAME) == NULL)
		return -1;
	/*如果第一个是默认的名称，第二个是非默认名称，则第二个排在第一个名称之前*/
	if(strstr(b, CUSTOM_PLAN_DEFAULT_ZH_NAME) == NULL)
		return 1;
	/*如果两个名称都是默认名称，则后缀整数小的排在前*/
	const char *p1 = a + strlen(CUSTOM_PLAN_DEFAULT_ZH_NAME);
	const char *p2 = b + strlen(CUSTOM_PLAN_DEFAULT_ZH_NAME);
	return atoi(p1) > atoi(p2) ? 1 : -1;
}

/**
 * 链表排序函数
 * 返回值：排序好的链表
 * 说明：排序原则，非默认名称排在前，默认名称按后缀升序排列
 * */
static GSList *
sort_extend_custom_plans(GSList *list)
{
	GSList *sort_list = g_slist_sort(list, plan_compare);

	return sort_list;
}


/**
 * 获取指定模板的自定义电源计划中的附加和隐藏电源计划名称
 * 返回值：由指定模板的自定义电源计划中的附加电源计划名组成的GSList
 * 本函数由gconf_client_get_extend_plans调用
 * */
static GSList *
gconf_client_get_extend_custom_plans_with_templet(GConfClient *client, const char *templet_name)
{
	GSList *list = NULL;
	char *plan_name = NULL;
	char buf[256] = {0};

	if(strcmp(templet_name, PLAN_NAME_BALANCE) == 0)
		sprintf(buf, "%s", POWER_CUSTOM_BALANCE_PATH);
	else if(strcmp(templet_name, PLAN_NAME_PERFORMANCE) == 0)
		sprintf(buf, "%s", POWER_CUSTOM_PERFORMANCE_PATH);
	else if(strcmp(templet_name, PLAN_NAME_SAVER) == 0)
		sprintf(buf, "%s", POWER_CUSTOM_SAVER_PATH);

	GSList *dirs = gconf_client_all_dirs(client, buf, NULL);
	GSList *tmp = dirs;
	while(tmp != NULL)
	{
		plan_name = gconf_client_get_string_key_with_path(client, (char *)tmp->data, "plan-name");
		if(gconf_client_is_extend_plan(client, plan_name))
			list = g_slist_append(list, (char *)plan_name);
		else
			free(plan_name);//释放不需要的内存

		tmp = tmp->next;
	}

	/*释放无用链表dirs*/
	g_slist_free(dirs);

	return list;
}

/**
 * 获取隐藏和附加电源计划名称
 * 返回值：GSList的元素由附加和隐藏电源计划的名称组成
 * */
GSList *
gconf_client_get_extend_plans(GConfClient *client)
{
	GSList *list = NULL;
	GSList *custom_list = NULL;
	GSList *tmp_list = NULL;
	char *plan_name = NULL;

	/*判断平衡计划是否是附加和隐藏计划*/
	plan_name = gconf_client_get_string_key_with_path(client, POWER_BALANCE_PATH, "plan-name");
	if(gconf_client_is_extend_plan(client, plan_name))
		list = g_slist_append(list, plan_name);
	else
		free(plan_name);//释放无用内存

	/*获取自定义平衡电源计划中属于附加和隐藏计划的名称链表*/
	custom_list = gconf_client_get_extend_custom_plans_with_templet(client, PLAN_NAME_BALANCE);

	/*获取自定义高性能电源计划中属于附加和隐藏计划的名称链表，合并追加到custom_list尾部*/
	tmp_list = gconf_client_get_extend_custom_plans_with_templet(client, PLAN_NAME_PERFORMANCE);
	custom_list = g_slist_concat(custom_list, tmp_list);

	/*获取自定义节能电源计划中属于附加和隐藏计划的名称链表，合并追加到custom_list尾部*/
	tmp_list = gconf_client_get_extend_custom_plans_with_templet(client, PLAN_NAME_SAVER);
	custom_list = g_slist_concat(custom_list, tmp_list);

	/*对附加和隐藏的自定义的电源计划链表排序后并入list尾部*/
	custom_list = sort_extend_custom_plans(custom_list);
	list = g_slist_concat(list, custom_list);

	/*判读高性能模式是否是附加和隐藏计划*/
	plan_name = gconf_client_get_string_key_with_path(client, POWER_PERFORMANCE_PATH, "plan-name");
	if(gconf_client_is_extend_plan(client, plan_name))
		list = g_slist_append(list, plan_name);
	else
		free(plan_name);//释放无用内存

	/*判断节能模式是否是附加和隐藏计划*/
	plan_name = gconf_client_get_string_key_with_path(client, POWER_SAVER_PATH, "plan-name");
	if(gconf_client_is_extend_plan(client, plan_name))
		list = g_slist_append(list, (char *)plan_name);
	else
		free(plan_name);//释放无用内存

	return list;
}

/**
 * 根据key_path获取key的名称
 * 返回值：key的basename, 如果不存在basename（以'/'字符结尾)则返回NULL。
 * 例如：key_path=“/apps/power-manager/custom/saver/custom_plan5/sleep-display-ac”, 返回值为"sleep-display-ac"
 * */
static char *
gconf_string_key_get_basename(const char *key_path)
{
	if(key_path == NULL)
		return NULL;

	/*查找最后一个'/'字符的位置，如果这个位置不是key_path的最后一个字符，则返回其后面内容的拷贝，否则返回NULL*/
	const char *p = strrchr(key_path, '/');
	if(abs(p - key_path) < strlen(key_path))
		return strdup(++p);

	return NULL;
}

/**
 * 根据key_name获取key的类型
 * 返回值：KeyType
 * */
static KeyType
gconf_string_key_get_type(const char *key_name)
{
	printf("%s : key_name[%s]\n", __func__, key_name);
	int i;
	/*使用balance的列表，遍历balance_keys，如果找到key_name的列表项，则返回该列表项的key_type*/
	for(i = 0; balance_keys[i].key_type != NULL_KEY; i++)
	{
		if(strcmp(balance_keys[i].key_name, key_name) == 0)
			return balance_keys[i].key_type;
	}

	/*在balance_keys中没有找到key_name，返回无效的类型*/
	return NULL_KEY;
}

/**
 * 将字符串类型的key_value，转换为enum类型的值
 * 返回值：成功返回CsdPowerActionType, 失败 -1
 * */
static CsdPowerActionType
gconf_string_key_to_action_type(const char *key_value)
{
	/*将相应的字符串转换为enum类型*/
	if(strcmp(key_value, "blank") == 0)
		return CSD_POWER_ACTION_BLANK;
	else if(strcmp(key_value, "suspend") == 0)
		return CSD_POWER_ACTION_SUSPEND;
	else if(strcmp(key_value, "shutdown") == 0)
		return CSD_POWER_ACTION_SHUTDOWN;
	else if(strcmp(key_value, "hibernate") == 0)
		return CSD_POWER_ACTION_HIBERNATE;
	else if(strcmp(key_value, "interactive") == 0)
		return CSD_POWER_ACTION_INTERACTIVE;
	else if(strcmp(key_value, "nothing") == 0)
		return CSD_POWER_ACTION_NOTHING;

	/*没有此类型对应的字符串，返回负数，表示失败*/
	return -1;
}

/**
 * 将enum类型的动作转换为字符串格式
 * 返回值：字符串格式的动作
 * */
static char *
gconf_enum_action_to_string(CsdPowerActionType action)
{
	if(action == CSD_POWER_ACTION_BLANK)
		return strdup("blank");
	else if(action == CSD_POWER_ACTION_SUSPEND)
		return strdup("suspend");
	else if(action == CSD_POWER_ACTION_SHUTDOWN)
		return strdup("shutdown");
	else if(action == CSD_POWER_ACTION_HIBERNATE)
		return strdup("hibernate");
	else if(action == CSD_POWER_ACTION_INTERACTIVE)
		return strdup("interactive");
	else if(action == CSD_POWER_ACTION_NOTHING)
		return strdup("nothing");

	return NULL;
}

/**
 * 将plan_name对应的gconf配置项同步到系统的gsettings下相应的配置项下
 * */
static void
gconf_client_sync_plan_settings_to_system(GConfClient *client, const char *plan_name)
{
	char *plan_path = gconf_client_get_plan_path_by_name(client, plan_name);
	printf("plan_path == %s\n", plan_path);
	GSList *entries = gconf_client_all_entries(client, plan_path, NULL);
	GSList *tmp = entries;
		printf("%s is enabled:\n", plan_name);
	while(tmp != NULL)
	{
		const char *key = gconf_entry_get_key(tmp->data);
		GConfValue *value = gconf_entry_get_value(tmp->data);
		switch(value->type)
		{
			case GCONF_VALUE_STRING:
				{
					const char *value_str = gconf_value_get_string(value);
					printf("%s == %s\n", gconf_string_key_get_basename(key), value_str);
					char *base_key = gconf_string_key_get_basename(key);

					/*如果配置项是plan-name则在忽略此选项,因为这个配置项是辅助配置项，用于记录当前计划的名称，并不是gsettings的配置项之一*/
					if(strcmp(base_key, "plan-name") == 0)
					{
						free(base_key);
						break;
					}

					/*
					 * 对于GCONF_VALUE_STRING的value有两种
					 * 一种是真正的字符串配置项，直接调用gsettings的string接口设置。
					 * 另一种是enum配置项，由于gconf没有enum类型的配置项，这里需要使用string转换为enum类型后调用gsettings的enum接口设置
					 * */
					KeyType key_type = gconf_string_key_get_type(base_key);
					printf("key_type[%d]\n", key_type);
					switch(key_type)
					{
						case STRING_KEY:
							g_settings_set_string(system_settings, base_key, value_str);
							break;
						case ENUM_KEY:
							{
							CsdPowerActionType action_type = gconf_string_key_to_action_type(value_str);
							g_settings_set_enum(system_settings, base_key, action_type);
							}
							break;
						default:
							break;
					}

					free(base_key);
				}
				break;
			case GCONF_VALUE_INT:
				{
					int value_int = gconf_value_get_int(value);
					printf("%s == %d \n", gconf_string_key_get_basename(key), value_int);

					char *base_key = gconf_string_key_get_basename(key);
					if(strcmp(base_key, "plan-name"))
						g_settings_set_int(system_settings, base_key, value_int);

					free(base_key);
				}
				break;
			case GCONF_VALUE_BOOL:
				{
					gboolean value_bool = gconf_value_get_bool(value);
					printf("%s == %d \n", gconf_string_key_get_basename(key), value_bool);

					char *base_key = gconf_string_key_get_basename(key);
					if(strcmp(base_key, "plan-name"))
						g_settings_set_boolean(system_settings, base_key, value_bool);

					free(base_key);
				}
				break;
			default:
				break;

		}
		tmp = tmp->next;
	}

	g_slist_free(entries);

	return ;
}

/**
 * 删除当前的配置项同步处理的信号函数
 * */
static void
gconf_client_disable_signals(GConfClient *client)
{
	/*获取当前使能的电源计划名称，并根据名称获取使能的电源计划配置路径*/
	char *plan_name = gconf_client_get_enabled_plan_name(client);

	/*将电源计划plan_name从监控范围中删除*/
	gconf_client_unmonitor_plan_by_name(client, plan_name);

	return ;
}

/**
 * 电源计划配置项同步处理的信号处理函数
 * */
static void
gconf_client_plan_notify_cb(GConfClient *client, guint func_id, GConfEntry *entry, gpointer user_data)
{
	const char *key = gconf_entry_get_key(entry);
	char *base_key= gconf_string_key_get_basename(key);

	/*如果配置项是plan-name则在忽略此选项,因为这个配置项是辅助配置项，用于记录当前计划的名称，并不是gsettings的配置项之一*/
	if(strcmp(base_key, "plan-name") == 0)
	{
		free(base_key);
		return;
	}

	/*将改变后的配置更新到系统的gsettings相应的配置项*/
	GConfValue *value = gconf_entry_get_value(entry);
	switch(value->type)
	{
		case GCONF_VALUE_STRING:
			{
				const char *value_str = gconf_value_get_string(value);
				printf("%s changed to  %s\n", base_key, value_str);
				/*
				 * 对于GCONF_VALUE_STRING的value有两种
				 * 一种是真正的字符串配置项，直接调用gsettings的string接口设置。
				 * 另一种是enum配置项，由于gconf没有enum类型的配置项，这里需要使用string转换为enum类型后调用gsettings的enum接口设置
				 * */
				KeyType key_type = gconf_string_key_get_type(base_key);
				printf("key_type[%d]\n", key_type);
				switch(key_type)
				{
					case STRING_KEY:
						g_settings_set_string(system_settings, base_key, value_str);
						break;
					case ENUM_KEY:
						{
							CsdPowerActionType action_type = gconf_string_key_to_action_type(value_str);
							g_settings_set_enum(system_settings, base_key, action_type);
						}
						break;
					default:
						break;
				}
			}
			break;
		case GCONF_VALUE_INT:
			{
				int value_int = gconf_value_get_int(value);
				printf("%s changed to  %d\n", base_key, value_int);
				g_settings_set_int(system_settings, base_key, value_int);
			}
			break;
		case GCONF_VALUE_BOOL:
			{
				gboolean value_bool = gconf_value_get_bool(value);
				printf("%s changed to  %d\n", base_key, value_bool);
				g_settings_set_boolean(system_settings, base_key, value_bool);
			}
			break;
		default:
			break;

	}

	free(base_key);

	return ;
}

/**
 * 为plan_name对应的gconf配置添加同步的信号处理函数
 * */
static void
gconf_client_add_signals_for_plan(GConfClient *client, const char *plan_name)
{

	char *plan_path = gconf_client_get_plan_path_by_name(client, plan_name);
	printf("%s: plan_path[%s]\n", __func__, plan_path);

	/*为plan_name的配置路径下的配置项添加信号处理函数*/
	gconf_client_notify_add(client, plan_path, gconf_client_plan_notify_cb, NULL, NULL, NULL);

	free(plan_path);

	return ;
}

/**
 * 取消监控电源计划plan_name的配置项的变化情况
 * */
static void
gconf_client_unmonitor_plan_by_name(GConfClient *client, const char *plan_name)
{
	/*获取plan_name的配置路径*/
	char *plan_path = gconf_client_get_plan_path_by_name(client, plan_name);
	
	/*将配置路径从监控范围中删除*/
	gconf_client_remove_dir(client, plan_path, NULL);

	free(plan_path);

	return ;
}

/**
 * 监控电源计划plan_name的配置路径下的配置项的变化
 * */
static void
gconf_client_monitor_plan_by_name(GConfClient *client, const char *plan_name)
{
	/*获取plan_name的配置路径*/
	char *plan_path = gconf_client_get_plan_path_by_name(client, plan_name);

	/*将配置路径添加到监控范围中*/
	gconf_client_add_dir(client, plan_path, GCONF_CLIENT_PRELOAD_NONE, NULL);

	free(plan_path);

	return ;
}

/**
 * 使能plan_name指定的电源计划
 * 说明：如果plan_name已经是使能的电源计划了，则什么都不做。
 * 如果plan_name不是首选计划，则先将plan_name设置为首选计划，再使能plan_name
 * 如果plan_name的模板是平衡模式，则将根路径下的balance-enabled配置项设置为TRUE，否则将balance-enabled设置为FALSE
 * */
void
gconf_client_enable_plan_by_name(GConfClient *client, const char *plan_name)
{
	/*如果已经是使能的电源计划，直接返回*/
	if(gconf_client_is_enabled_plan(client, plan_name))
	{
		printf("%s is already enabled\n", plan_name);
		return ;
	}

	/*取消当前使能的电源计划同步信号处理函数*/
	gconf_client_disable_signals(client);

	/*如果plan_name还不是首选计划，先将plan_name设置为首选计划*/
	if(!gconf_client_is_preference_plan(client, plan_name))
	{
		gconf_client_set_preference_plan(client, plan_name);
	}

	/*
	 * 根据plan_name的模板设置配置POWER_ROOT_PATH下的balance-enabled配置项
	 * 如果plan_name的模板是balance则将balance-enbaled设置为TRUE，否则为FALSE
	 * */
	char *plan_templet = gconf_client_get_templet_by_name(client, plan_name);
	if(strcmp(plan_templet, PLAN_NAME_BALANCE) == 0)
		gconf_client_set_bool_key_with_path(client, POWER_ROOT_PATH, "balance-enabled", TRUE, NULL);
	else
		gconf_client_set_bool_key_with_path(client, POWER_ROOT_PATH, "balance-enabled", FALSE, NULL);

	/*将plan_name的电源配置信息同步到系统的gsettings配置中，使之生效*/
	gconf_client_sync_plan_settings_to_system(client, plan_name);

	/*将plan_name的配置路径加入监控之中，以便当plan_name的配置项变化时，能够及时的同步到gsettings中*/
	gconf_client_monitor_plan_by_name(client, plan_name);

	return ;
}

/**
 * 根据当前默认自定义电源计划状态，创建新的自定义电源计划名称
 * */
static const char *
gconf_client_make_default_custom_plan_path(GConfClient *client)
{
	int i;
	char buf[256] = {0};
	/*获取当前的默认自定义电源计划个数*/
	int num = gconf_client_get_default_custom_plans_num(client);

	/*先遍历1到num确认是否存在已经删除的默认自定义计划, 如果有则重复使用*/
	for(i = 1; i <= num; i++)
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%s%d", CUSTOM_PLAN_DEFAULT_NAME, i);
		if(! gconf_client_custom_plan_path_exists(client, buf))
			return strdup(buf);
	}

	/*1到num中不存在已经删除的自定义计划，更新默认自定义电源计划个数*/
	num = i;
	gconf_client_set_default_custom_plans_num(client, num);

	memset(buf, 0, sizeof(buf));
	sprintf(buf, "%s%d", CUSTOM_PLAN_DEFAULT_NAME, num);

	return strdup(buf);
}

/**
 * 根据模板的名称，为新创建的自定义电源计划创建配置项
 * templet_plan:拷贝的模板名称(eg:balance、custom、performance)
 * custom_plan:自定义的电源计划名(eg: MyCustomPlan)
 * plan_name:新创建的电源计划的名称
 * 如果目标计划已经存在，则不再拷贝
 * */
static void
gconf_client_create_plan_keys_with_templet(GConfClient *client, const char *templet_plan, const char *plan_path, const char *plan_name)
{
	char src_buf[128] = {0};
	char dest_buf[128] = {0};

	if(strcmp(templet_plan, PLAN_NAME_BALANCE) == 0 )
	{
		sprintf(dest_buf, "%s/%s", POWER_CUSTOM_BALANCE_PATH, plan_path);
		sprintf(src_buf, "%s", POWER_BALANCE_PATH);
	}
	else if(strcmp(templet_plan, PLAN_NAME_PERFORMANCE) == 0)
	{
		sprintf(dest_buf, "%s/%s", POWER_CUSTOM_PERFORMANCE_PATH, plan_path);
		sprintf(src_buf, "%s", POWER_PERFORMANCE_PATH);
	}
	else if(strcmp(templet_plan, PLAN_NAME_SAVER) == 0)
	{
		sprintf(dest_buf, "%s/%s", POWER_CUSTOM_SAVER_PATH, plan_path);
		sprintf(src_buf, "%s", POWER_SAVER_PATH);
	}
	
	/*如果源目录或目标目录有一个为空，则返回*/
	if((strlen(src_buf) == 0) || (strlen(dest_buf) == 0))
		return ;

	/*如果目标目录不存在，则拷贝源目录的内容到目标目录下*/
	if(!gconf_client_dir_exists(client, dest_buf, NULL))
		gconf_client_dir_copy(client, src_buf, dest_buf);

	/*设置新创建的电源计划的名称为plan_name*/
	gconf_client_set_plan_name(client, dest_buf, plan_name);

	return ;
}


/**
 * 查找在配置路径plan_path下，是否存在名称叫做plan_name的电源计划
 * 返回值：TRUE 存在， FALSE 不存在
 * */
static gboolean
gconf_client_plan_name_exists_with_path(GConfClient *client,  const char *plan_path, const char *plan_name)
{
	gboolean ret = FALSE;

	char *str = gconf_client_get_string_key_with_path(client, plan_path, "plan-name");

	if(strcmp(str, plan_name) == 0 )
		ret = TRUE;

	free(str);

	return ret;
}

/**
 * 查找配置路径plan_path下的子目录是否存在名称叫做plan_name的电源计划
 * plan_path:配置路径，一般传值为（POWER_CUSTOM_BALANCE_PATH、POWER_CUSTOM_PERFORMANCE_PATH、POWER_CUSTOM_SAVER_PATH)
 * 返回值：TRUE 存在， FALSE 不存在
 * 说明：这个函数用与确定在自定义电源计划中是否已经存在名称plan_name的电源计划
 * */
static gboolean
gconf_client_plan_name_exists_under_path(GConfClient *client, const char *plan_path, const char *plan_name)
{
	gboolean ret = FALSE;

	GSList *subdirs = gconf_client_all_dirs(client, plan_path, NULL);
	
	GSList *tmp = subdirs;
	while(tmp != NULL)
	{
		if(gconf_client_plan_name_exists_with_path(client, (const char *)tmp->data, plan_name))
		{
			ret = TRUE;
			break;
		}

		tmp = tmp->next;
	}

	g_slist_free(subdirs);

	/*所有子目录遍历完毕，不存在名字叫做plan_name的电源计划，返回FALSE*/
	return ret;
}

/**
 * 判断一个是否已经存在自定义电源计划名plan_name
 * 返回值：存在返回TRUE， 不存在返回FALSE*/
static gboolean
gconf_client_custom_plan_exists(GConfClient *client, const char *plan_name)
{
	/*如果plan_name与平衡模式、高性能模式或节能模式的名字相同，则存在返回*/
	if(gconf_client_plan_name_exists_with_path(client, POWER_BALANCE_PATH, plan_name))
		return TRUE;
	else if(gconf_client_plan_name_exists_with_path(client, POWER_PERFORMANCE_PATH, plan_name))
		return TRUE;
	else if(gconf_client_plan_name_exists_with_path(client, POWER_SAVER_PATH, plan_name))
		return TRUE;

	/*确定plan_name是否与已经创建的自定义平衡模式重名*/
	if(gconf_client_plan_name_exists_under_path(client, POWER_CUSTOM_BALANCE_PATH, plan_name))
		return TRUE;
	else if(gconf_client_plan_name_exists_under_path(client, POWER_CUSTOM_PERFORMANCE_PATH, plan_name))
		return TRUE;
	else if(gconf_client_plan_name_exists_under_path(client, POWER_CUSTOM_SAVER_PATH, plan_name))
		return TRUE;

	/*不存在这个名称的自定义电源计划，返回FALSE*/
	return FALSE;
}


/**
 * 创建自定义电源管理计划
 * 第2个参数templet_name是模板的名称， 第3个参数plan_name是自定义计划的名称
 * 如果第2个参数为NULL, 则使用平衡模式为模板创建新的电源计划
 * 如果第3个参数为NULL, 则使用custom_plan[1 2 3....]为自定义电源计划的名称, 后缀的数字自动递增
 *  */
void
gconf_client_create_custom_plan(GConfClient *client, const char *templet_name, const char *plan_name)
{
	const char *plan_path = NULL;

	/*默认以平衡模式为模板, 如果指定了模板名称，则使用指定的模板*/
	const char *templet_plan = CUSTOM_DEFAULT_TEMPLET;
	if(templet_name != NULL)
		templet_plan = templet_name;

	/*为新增的电源计划创建配置存储路径*/
	plan_path = gconf_client_make_default_custom_plan_path(client); 

	/*如果要创建的存储路径已经存在则直接返回,这里应该一直不存在*/
	if(gconf_client_custom_plan_path_exists(client, plan_path))
	{
		printf("custom plan path %s already exists, now return \n", plan_path);
		return ;
	}

	/*如果没有指定模板名称*/
	if(plan_name == NULL)
	{
		plan_name = gconf_client_make_zh_name_with_custom_plan_path(client, plan_path);
		printf("plan_name == %s\n", plan_name);
	}

	/*如果传递的电源计划名称已经存在，不再重复创建，直接返回*/
	if(gconf_client_custom_plan_exists(client, plan_name))
	{
		printf("custom plan %s already exists, now return \n", plan_name);
		return ;
	}

	/*根据电源计划模板，拷贝相应的entries到新的电源计划中*/
	printf("create new plan path: %s which is a copy of %s\n", plan_path, templet_name);
	gconf_client_create_plan_keys_with_templet(client, templet_plan, plan_path, plan_name);

	/*为新创建的电源计划配置项添加同步信号处理函数*/
	gconf_client_add_signals_for_plan(client, plan_name);

	return ;
}

/**
 * 根据自定义电源计划的名称plan_name获取其配置存储路径
 * 返回值：自定义电源计划plan_name的配置存储路径
 * */
static char *
gconf_client_get_custom_plan_path_with_name(GConfClient *client, const char *plan_path, const char *plan_name)
{
	char *str = NULL;

	GSList *subdirs = gconf_client_all_dirs(client, plan_path, NULL);
	GSList *tmp = subdirs;
	while(tmp != NULL)
	{
		/*找到plan_name的path，退出循环*/
		if(gconf_client_plan_name_exists_with_path(client, (const char *)tmp->data, plan_name))
		{
			str =  strdup((const char *)tmp->data);
			break;
		}

		tmp = tmp->next;
	}

	g_slist_free(subdirs);

	return str;
}


/**
 * 根据名称，获取电源管理计划的配置保存路径*/
static char *
gconf_client_get_plan_path_by_name(GConfClient *client, const char *plan_name)
{
	char *str = NULL;

	if(plan_name == NULL)
		return NULL;

	/*如果是平衡、高性能、节能计划，则返回固定的相应计划的路径*/
	if (gconf_client_plan_name_exists_with_path(client, POWER_BALANCE_PATH, plan_name))
		return strdup(POWER_BALANCE_PATH);
	else if (gconf_client_plan_name_exists_with_path(client, POWER_PERFORMANCE_PATH, plan_name))
		return strdup(POWER_PERFORMANCE_PATH);
	else if (gconf_client_plan_name_exists_with_path(client, POWER_SAVER_PATH, plan_name))
		return strdup(POWER_SAVER_PATH);

	if((str = gconf_client_get_custom_plan_path_with_name(client, POWER_CUSTOM_BALANCE_PATH, plan_name)) != NULL)
		return str;
	else if((str = gconf_client_get_custom_plan_path_with_name(client, POWER_CUSTOM_PERFORMANCE_PATH, plan_name)) != NULL)
		return str;
	else if((str = gconf_client_get_custom_plan_path_with_name(client, POWER_CUSTOM_SAVER_PATH, plan_name)) != NULL)
		return str;

	/*如果以上路径都不能找到该计划，则证明该计划不存在，返回NULL*/
	return NULL;
}

/**
 * 获取plan_name指定的电源计划所使用的模板名称
 * 返回值：plan_name的模板名称
 * */
static char *
gconf_client_get_templet_by_name(GConfClient *client, const char *plan_name)
{
	char plan_path[256] = {0};
	/*根据plan_name获取其配置存储路径*/
	char * str = gconf_client_get_plan_path_by_name(client, plan_name); 

	/*如果是默认平衡/高性能/节能计划的配置路径，模板为平衡/高性能/节能模式*/
	if(strcmp(str, POWER_BALANCE_PATH) == 0)
	{
		free(str);
		return strdup(PLAN_NAME_BALANCE);
	}
	else if(strcmp(str, POWER_PERFORMANCE_PATH) == 0)
	{
		free(str);
		return strdup(PLAN_NAME_PERFORMANCE);
	}
	else if(strcmp(str, POWER_SAVER_PATH) == 0)
	{
		free(str);
		return strdup(PLAN_NAME_SAVER);
	}

	/*将配置路径存放到本地buf中后释放str*/
	sprintf(plan_path, "%s", str);

	free(str);

	/*
	 * 倒数第二个'/'与倒数第一个'/'之间的部分就是plan_name的模板名称
	 * 例如：/apps/power-manager/custom/performance/custom_plan2的模板名称为performance
	 * */
	char *p = strrchr(plan_path, '/');
	*p = '\0';
	p = strrchr(plan_path, '/');
	
	return strdup(++p);
}

/**
 * 将plan_name指定的电源计划设置为首选项电源计划
 * 说明：如果plan_name的模板是平衡模式，
 * 则将plan_name设置为首选项的第一条电源计划
 * 否则，将plan_name设置为首选项的第二条电源计划
 * */
static void
gconf_client_set_preference_plan(GConfClient *client, const char *plan_name)
{
	const char *templet = gconf_client_get_templet_by_name(client, plan_name);
	printf("templet of %s == %s\n", plan_name, templet);
	if(strcmp(templet, PLAN_NAME_BALANCE) == 0)
		gconf_client_set_string_key_with_path(client, POWER_ROOT_PATH, "balance-key", plan_name, NULL);
	else
		gconf_client_set_string_key_with_path(client, POWER_ROOT_PATH, "second-key", plan_name, NULL);
}

/**
 * 判断一个plan_name指定的电源计划是否可以删除
 * 返回值：TRUE 可以删除， FALSE 不可以删除
 * */
gboolean
gconf_client_plan_is_deletable(GConfClient *client, const char *plan_name)
{
	gboolean ret = TRUE;

	/*如果plan_name是默认的平衡、高性能或节能模式则不可删除*/
	if((strcmp(plan_name, PLAN_ZH_NAME_BALANCE) == 0) 
			|| (strcmp(plan_name, PLAN_ZH_NAME_PERFORMANCE) == 0)
			|| (strcmp(plan_name, PLAN_ZH_NAME_SAVER) == 0))
		ret = FALSE;

	/*如果plan_name正在使能, 则不可删除*/
	if(gconf_client_is_enabled_plan(client, plan_name))
	{
		printf("%s is enabled plan can't be deleted!\n", plan_name);
		ret = FALSE;
	}

	return ret;
}

/**
 * 根据电源计划名称，删除一个自定义电源计划
 * */
void
gconf_client_delete_custom_plan_by_name(GConfClient *client, const char *plan_name)
{
	if(plan_name == NULL)
		return ;

	/*如果电源计划不可删除，直接返回*/
	if(!gconf_client_plan_is_deletable(client, plan_name))
	{
		printf("%s can't deleted !\n", plan_name);
		return;
	}

	/*如果要删除的电源计划是首选计划, 删除前首先更新首选计划*/
	if(gconf_client_is_preference_plan(client, plan_name))
	{
		const char *templet_name = gconf_client_get_templet_by_name(client, plan_name);
		if(strcmp(templet_name, PLAN_NAME_BALANCE) == 0)
			gconf_client_set_preference_plan(client, PLAN_ZH_NAME_BALANCE);
		else if(strcmp(templet_name, PLAN_NAME_PERFORMANCE) == 0)
			gconf_client_set_preference_plan(client, PLAN_ZH_NAME_PERFORMANCE);
		else if(strcmp(templet_name, PLAN_NAME_SAVER) == 0)
			gconf_client_set_preference_plan(client, PLAN_ZH_NAME_SAVER);
	}

	/*真正删除plan_name的配置信息*/
	char *plan_path = gconf_client_get_plan_path_by_name(client, plan_name);
	if(plan_path != NULL)
		gconf_client_recursive_unset(client, plan_path, GCONF_UNSET_INCLUDING_SCHEMA_NAMES, NULL);

	/*释放不再使用的字符串*/
	free(plan_path);

	return ;
}

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
gconf_client_set_plan_key_value(GConfClient *client, const char *plan_name, const char *key, const char *value)
{
	char *plan_path = gconf_client_get_plan_path_by_name(client, plan_name);
	KeyType key_type = gconf_string_key_get_type(key);
	switch(key_type)
	{
		case STRING_KEY:
			gconf_client_set_string_key_with_path(client, plan_path, key, value, NULL);
			break;
		case ENUM_KEY:
			{
			gconf_client_set_string_key_with_path(client, plan_path, key, value, NULL);
			}
			break;
		case INT_KEY:
			gconf_client_set_int_key_with_path(client, plan_path, key, atoi(value), NULL);
			break;
		case BOOL_KEY:
			gconf_client_set_bool_key_with_path(client, plan_path, key, strcmp(value, "TRUE") == 0 ? TRUE : FALSE, NULL);
			break;
		default:
			break;
	}

	return ;
}

/**
 * 设置/修改电源计划plan_name的string类型的配置项key的值为字符串value
 * */
void
gconf_client_set_string_plan_key_value(GConfClient *client, const char *plan_name, const char *key, const char *value)
{
	char *plan_path = gconf_client_get_plan_path_by_name(client, plan_name);
	gconf_client_set_string_key_with_path(client, plan_path, key, value, NULL);

	return;
}

/**
 * 设置/修改电源计划plan_name的int类型的配置项key的值为整形数值value
 * */
void
gconf_client_set_int_plan_key_value(GConfClient *client, const char *plan_name, const char *key, int value)
{
	char *plan_path = gconf_client_get_plan_path_by_name(client, plan_name);
	gconf_client_set_int_key_with_path(client, plan_path, key, value, NULL);

	return;
}

/**
 * 设置/修改电源计划plan_name的bool类型的配置项key的值为布尔类型值value
 * */
void
gconf_client_set_bool_plan_key_value(GConfClient *client, const char *plan_name, const char *key, gboolean value)
{
	char *plan_path = gconf_client_get_plan_path_by_name(client, plan_name);
	gconf_client_set_bool_key_with_path(client, plan_path, key, value, NULL);

	return;
}

/**
 * 设置/修改电源计划plan_name的enum类型的配置项key的值为enum类型value
 * 说明：gconf不支持enum类型的配置项，所以是以string类型存储的，提取时会自动从string类型转换为enum类型
 * */
void
gconf_client_set_enum_plan_key_value(GConfClient *client, const char *plan_name, const char *key, int value)
{
	char *plan_path = gconf_client_get_plan_path_by_name(client, plan_name);
	char *value_str = gconf_enum_action_to_string(value);

	gconf_client_set_string_key_with_path(client, plan_path, key, value_str, NULL);

	return;
}

/**
 * 获取电源计划plan_name的配置项key的值
 * plan_name：key所在的电源计划的名称
 * key：要获取的配置项的名称
 * 返回值：字符串类型
 * 对于非字符串类型的key值，转化为字符串类型，例如："TRUE"、"FALSE"、"suspend"、"hibernate"
 * */
char *
gconf_client_get_plan_key_value(GConfClient *client, const char *plan_name, const char *key)
{
	char *key_value = NULL;

	/*根据plan_name获取该电源计划的配置路径*/
	char *plan_path = gconf_client_get_plan_path_by_name(client, plan_name);
	
	/*获取key的类型，并根据该类型到电源计划的配置路径下获取key的值*/
	KeyType key_type = gconf_string_key_get_type(key);
	switch(key_type)
	{
		case ENUM_KEY:
		case STRING_KEY:
			{
				key_value = gconf_client_get_string_key_with_path(client, plan_path, key);
			}
			break;
		case INT_KEY:
			{
				int value_int = gconf_client_get_int_key_with_path(client, plan_path, key);
				char buf[8] = {0};
				sprintf(buf, "%d", value_int);
				key_value = strdup(buf);
			}
			break;
		case BOOL_KEY:
			{
				gboolean value_bool = gconf_client_get_bool_key_with_path(client, plan_path, key);
				if(value_bool)
					key_value = strdup("TRUE");
				else
					key_value = strdup("FALSE");
			}
			break;
		default:
			break;
	}

	/*释放无用内存*/
	free(plan_path);

	return key_value;
}

/**
 * 获取电源计划plan_name下的string类型的配置项key的值
 * 返回值：key的值
 * */
char *
gconf_client_get_string_plan_key_value(GConfClient *client, const char *plan_name, const char *key)
{
	/*根据电源计划名称plan_name获取电源计划的存储路径*/
	char *plan_path = gconf_client_get_plan_path_by_name(client, plan_name);

	/*直接根据路径获取字符串类型key的值*/
	char *key_value = gconf_client_get_string_key_with_path(client, plan_path, key);

	free(plan_path);

	return key_value;
}

/**
 * 获取电源计划plan_name下的int类型的配置项key的值
 * 返回值：key的值
 * */
int
gconf_client_get_int_plan_key_value(GConfClient *client, const char *plan_name, const char *key)
{
	/*根据电源计划名称plan_name获取电源计划的存储路径*/
	char *plan_path = gconf_client_get_plan_path_by_name(client, plan_name);

	/*直接根据路径获取字符串类型key的值*/
	int key_value = gconf_client_get_int_key_with_path(client, plan_path, key);

	free(plan_path);

	return key_value;
}

/**
 * 获取电源计划plan_name下的bool类型的配置项key的值
 * 返回值：key的值
 * */
gboolean
gconf_client_get_bool_plan_key_value(GConfClient *client, const char *plan_name, const char *key)
{
	/*根据电源计划名称plan_name获取电源计划的存储路径*/
	char *plan_path = gconf_client_get_plan_path_by_name(client, plan_name);

	/*直接根据路径获取字符串类型key的值*/
	gboolean key_value = gconf_client_get_bool_key_with_path(client, plan_path, key);

	free(plan_path);

	return key_value;
}

/**
 * 获取电源计划plan_name的enum类型的配置项的值
 * 返回值：key的值
 * */
CsdPowerActionType
gconf_client_get_enum_plan_key_value(GConfClient *client, const char *plan_name, const char *key)
{
	/*
	 * gconf不支持enum类型配置项，enum类型的配置项在gconf配置中以string类型存储，
	 * 因此，对于enum类型的key，先提取其string类型的值
	 * */
	char *key_value = gconf_client_get_string_plan_key_value(client, plan_name, key);
	
	/*将string类型的值转换为enum类型后返回*/
	return gconf_string_key_to_action_type(key_value);
}

int main(int argc, char **argv)
{
	loop = g_main_loop_new(NULL, FALSE);

	GConfClient *client;
	client = gconf_client_get_default();

	/*初始化，用于初始化配置信息, 包括首次运行创建默认的电源计划*/
	gconf_client_init(client);

	//读取配置信息
	read_conf(client);


	/*目录拷贝测试， 第2个参数是源目录， 第3个参数是目标目录*/
	//gconf_client_dir_copy(client, POWER_BALANCE_PATH, "/apps/power-manager/custom/balance/custom_balance1");


	/*
	 * 创建自定义电源管理计划测试, 第2个参数是模板的名称， 第3个参数是自定义计划的名称
	 * 如果第2个参数为NULL, 则使用平衡模式为模板创建新的电源计划
	 * 如果第3个参数为NULL, 则使用custom_plan[1 2 3....]为自定义电源计划的名称, 后缀的数字自动递增
	 * */
	gconf_client_create_custom_plan(client, NULL, NULL);
	gconf_client_create_custom_plan(client, PLAN_NAME_PERFORMANCE, "Mycustomplan");
	gconf_client_create_custom_plan(client, PLAN_NAME_PERFORMANCE, NULL);
	gconf_client_create_custom_plan(client, PLAN_NAME_SAVER, NULL);
	gconf_client_create_custom_plan(client, PLAN_NAME_SAVER, "Mycustomplan");
	gconf_client_create_custom_plan(client, PLAN_NAME_SAVER, "我的自定义节能计划");
	gconf_client_create_custom_plan(client, PLAN_NAME_SAVER, "我的新自定义节能计划");


	/*测试将一个电源计划设置为首选项电源计划*/
	gconf_client_set_preference_plan(client, "我的自定义节能计划");
	gconf_client_set_preference_plan(client, "高性能");
	gconf_client_set_preference_plan(client, "自定义电源计划1");


	/*测试删除一个自定义的电源计划*/
	//gconf_client_delete_custom_plan_by_name(client, "自定义电源计划1");
	gconf_client_delete_custom_plan_by_name(client, "Mycustomplan");
	//gconf_client_delete_custom_plan_by_name(client, "我的自定义节能计划");
	//gconf_client_delete_custom_plan_by_name(client, "平衡");
	//gconf_client_delete_custom_plan_by_name(client, "高性能");


	/*测试通过电源管理计划名获取计划的配置路径*/
	const char *conf_path = gconf_client_get_plan_path_by_name(client, "Mycustomplan");
	printf("path[%s]\n", conf_path);
	printf("configure path of 我的自定义节能计划[%s]\n", gconf_client_get_plan_path_by_name(client, "我的自定义节能计划"));
	printf("configure path of Mycustomplan[%s]\n", gconf_client_get_plan_path_by_name(client, "Mycustomplan"));
	printf("configure path of 平衡[%s]\n", gconf_client_get_plan_path_by_name(client, "平衡"));
	printf("configure path of 高性能[%s]\n", gconf_client_get_plan_path_by_name(client, "高性能"));
	printf("configure path of 节能[%s]\n", gconf_client_get_plan_path_by_name(client, "节能"));


	/*测试获取首选项电源计划名称*/
	printf("first preference plan name: %s\n", gconf_client_get_first_preference_plan_name(client));
	printf("second preference plan name: %s\n", gconf_client_get_second_preference_plan_name(client));
	
	/*测试获取使能的电源计划名称*/
	printf("Enabled power plan: %s\n", gconf_client_get_enabled_plan_name(client));



	/*测试读取附加和隐藏计划*/
	GSList *plans = gconf_client_get_extend_plans(client);
	gconf_client_read_extend_plans(client, plans);


	/*测试使能一个电源计划*/
	gconf_client_enable_plan_by_name(client, "我的自定义节能计划");
	gconf_client_enable_plan_by_name(client, "平衡");
	gconf_client_enable_plan_by_name(client, "我的自定义节能计划");
	gconf_client_enable_plan_by_name(client, "平衡");
	gconf_client_enable_plan_by_name(client, "我的自定义节能计划");
	//gconf_client_enable_plan_by_name(client, "平衡");
	//gconf_client_enable_plan_by_name(client, "Mycustomplan");

	/*测试根据key绝对路径获取key的名称*/
	//char * tmp_str = gconf_string_key_get_basename("/apps/power-manager/custom/saver/custom_plan5/sleep-display-ac");
	char * tmp_str = gconf_string_key_get_basename("aaa");
	printf("tmp_str == %s\n", tmp_str);

	/*测试根据电源计划名plan_name修改其配置项*/
	gconf_client_set_plan_key_value(client, "平衡", "sleep-display-ac", "200");

	/*测试获取电源计划plan_name的配置项key_name的value*/
	char *sleep_display_ac = gconf_client_get_plan_key_value(client, "平衡", "sleep-display-ac");
	printf("sleep-display-ac in gconf  is %s\n", sleep_display_ac);
	free(sleep_display_ac);
	char *button_power = gconf_client_get_string_plan_key_value(client, "平衡", "button-power");
	printf("string button-power in gconf  is %s\n", button_power);
	free(sleep_display_ac);
	//CsdPowerActionType button_sleep = gconf_client_get_enum_plan_key_value(client, "平衡", "button-sleep");
	//printf("enum button-sleep in gconf is %d\n", button_sleep);
	int value_int = gconf_client_get_int_plan_key_value(client, "平衡", "sleep-display-ac");
	printf("int sleep-display-ac in gconf  is %d \n", value_int);
	gboolean lock_on_suspend = gconf_client_get_bool_plan_key_value(client, "平衡", "lock-on-suspend");
	printf("bool lock-on-suspend in gconf is %d\n", lock_on_suspend);

#if 0
	/*修改电源计划配置项测试*/
	gconf_client_set_bool_key_with_path(client,	POWER_ROOT_PATH, "balance-enabled", FALSE, NULL);
	gconf_client_set_bool_key_with_path(client,	conf_path, "enabled", FALSE, NULL);


	/*递归清除电源管理路径所有内容*/
	GError *error;
	gconf_client_recursive_unset(client, "/apps/power-manager", GCONF_UNSET_INCLUDING_SCHEMA_NAMES, &error);
#endif

	g_main_loop_run(loop);
	g_main_loop_unref(loop);

	return 0;
}
