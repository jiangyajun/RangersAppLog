//
//  BDTracker.h
//  Applog
//
//  Created by bob on 2019/1/20.
//

#import <Foundation/Foundation.h>
#import "BDAutoTrackConfig.h"

NS_ASSUME_NONNULL_BEGIN

/*!
 请求URL type
 */
typedef NSInteger BDAutoTrackRequestURLType NS_TYPED_ENUM;

FOUNDATION_EXTERN BDAutoTrackRequestURLType const BDAutoTrackRequestURLRegister; /// 注册
FOUNDATION_EXTERN BDAutoTrackRequestURLType const BDAutoTrackRequestURLActivate; /// 激活
FOUNDATION_EXTERN BDAutoTrackRequestURLType const BDAutoTrackRequestURLSettings; /// 基本配置
FOUNDATION_EXTERN BDAutoTrackRequestURLType const BDAutoTrackRequestURLABTest;   /// ABTest配置
FOUNDATION_EXTERN BDAutoTrackRequestURLType const BDAutoTrackRequestURLLog;      /// 日志上报
FOUNDATION_EXTERN BDAutoTrackRequestURLType const BDAutoTrackRequestURLLogBackup;/// 日志上报备用

/*! @abstract 自定义请求链接，把相关请求发送到对应的自定义的URL上
 @param vendor 地区
 @param requestURLType 上面的六个枚举值
 @discussion 一般情况不需要实现；如果实现，针对想改变的枚举值返回一个准确的URL，其他不想修改的返回nil即可
 @result 返回自定义的URL
 */
typedef NSString * _Nullable (^BDAutoTrackRequestURLBlock)(BDAutoTrackServiceVendor vendor, BDAutoTrackRequestURLType requestURLType);

/*! @abstract ABTest配置拉取回调
 @param ABTestEnabled ABTest是否开启
 @param allConfigs 后台返回的全部配置
 @discussion ABTestEnabled如果未开启，则，即使有config，SDK也会返回默认值；如果无网络或者其他原因注册失败，不会回调
 */
typedef void(^BDAutoTrackABTestFinishBlock)(BOOL ABTestEnabled, NSDictionary * _Nullable  allConfigs);

/*! @abstract 设备注册回调
 @param deviceID did
 @param installID iid
 @param ssID ssid
 @param userUniqueID userUniqueID
 @discussion 可能为空，如果无网络或者其他原因注册失败，不会回调
 */
typedef void(^BDAutoTrackRegisterFinishBlock)(NSString * _Nullable deviceID, NSString * _Nullable installID, NSString * _Nullable ssID, NSString *_Nullable userUniqueID);

/*! @abstract 自定义上报信息
 @discussion 每次上报都会回调，设置一次即可，格式要求同日志要求，需要可序列化；如果无法序列化，会被丢弃
 @result NSDictionary custom数据
 */
typedef NSDictionary<NSString*, id> *_Nonnull (^BDAutoTrackCustomHeaderBlock)(void);

/*!
 BDAutoTrack日志上报功能的类
 */
@interface BDAutoTrack : NSObject

/*! @abstract
SDK版本号.
 */
@property (class, nonatomic, copy, readonly) NSString *sdkVersion;

/*! @abstract
 针对每个AppID 设备唯一device ID. 非数字，随机字符串，一般情况请不要存储，从SDK接口获取即可。
 */
@property (nonatomic, copy, readonly, nullable) NSString *deviceID __attribute__((deprecated("过期属性，后续版本可能不会再提供此接口")));

///*! @abstract
// 针对每个AppID 设备唯一byteDance device ID. 非数字，随机字符串，一般情况请不要存储，从SDK接口获取即可。
// @discussion 针对不同的AppID如果是同一个集团，则有可能是一样的。原来的device ID已经废弃移除
// @discussion 特别说明，bytedanceDeviceID和deviceID 不是一个值，是不一样的
// */
//@property (nonatomic, copy, readonly, nullable) NSString *bytedanceDeviceID;

/*! @abstract
 安装ID，是数字字符串，可以转化为数字。
 */
@property (nonatomic, copy, readonly, nullable) NSString *installID;

/*! @abstract
 数说ID，一般情况下，一个自然人的ssid是一致的。
 */
@property (nonatomic, copy, readonly, nullable) NSString *ssID;

/*! @abstract
 通过[BDAutoTrack setCurrentUserUniqueID:]接口传入到SDK内部的uuid
 */
@property (nonatomic, copy, readonly, nullable) NSString *userUniqueID;

/*! @abstract 初始化的AppID
 */
@property (nonatomic, copy, readonly) NSString *appID;

/*! @abstract 初始化方法，初始化一个Track实例
 @param config 初始化配置，AppID AppName Channel是必须的
 @discussion 初始化接口可以重复调用，会返回同一个实例，推荐返回之后，引用住这个实例，下次上报方便使用。
 @result 一个Track实例
 */
+ (nullable instancetype)trackWithConfig:(BDAutoTrackConfig *)config;

/*! @abstract 根据App ID获取一个已经初始化过的Track实例
 @param appID AppID
 @discussion 此接口需要先调用 `[BDAutoTrack trackWithConfig:]`初始化过实例，才能获取到
 @result 一个Track实例
 */
+ (nullable instancetype)trackWithAppID:(NSString *)appID;

/*! @abstract 开始埋点和无埋点上报
 @discussion 需要在`[BDAutoTrack trackWithConfig:]`初始化之后手动调用此方法。调用此方法之前，可以做一些额外的配置
 */
- (void)startTrack;

/*! @abstract 手动触发获取SDK上报配置值请求
 @discussion 手动触发请求
 */
- (void)startFetchTrackerConfiguration;

/*! @abstract userAgent
 @discussion 每次启动SDK的时候设置一次，发生变化的时候设置即可。一般不会发生变化，只需要设置一次即可
 @param userAgent 日志上报HTTP/HTTPS 请求里面的 userAgent
 */
- (void)setUserAgent:(nullable NSString *)userAgent;

/*! @abstract UserUniqueID发生变化时设置
 @discussion 有值，则设置为ID值；登出 请调用 [BDAutoTrack clearUserUniqueID] 或者传 nil
 @discussion SDK会保存，因此只需要变化的时候设置。
 @param uniqueID 用户id，如无特殊需求，请勿传 空字符串 或者 全是空格的字符串
 */
- (void)setCurrentUserUniqueID:(nullable NSString *)uniqueID;

/*! @abstract 登出 logout
 @discussion 登出 logout
 */
- (void)clearUserUniqueID;

/*! @abstract 设置上报Host地区，有国内、新加坡、美东三个选项
 @discussion 发生变化可以更新，不需要一直重复设置
 @param serviceVendor 地区
 */
- (void)setServiceVendor:(BDAutoTrackServiceVendor)serviceVendor;

/*! @abstract 设置自定义的URL回调
 @param requestURLBlock 回调block，设置一次即可，不需要多次设置
 @discussion requestURLBlock 会覆盖之前的初始化或者上一次设置的回调，如果为nil会清空回调
 @discussion block在初始化之前 设置一次即可，每次拉取请求都会回调，请勿一直重复设置
 */
- (void)setRequestURLBlock:(nullable BDAutoTrackRequestURLBlock)requestURLBlock;

/*! @abstract 地区
 @discussion 如果设置过，会保存值，直到下次改变或者清空
 @discussion 如果没有值，默认会读取 `[[NSLocale currentLocale] objectForKey:NSLocaleCountryCode]`
 @discussion 发生变化时候请调用 `+[BDAutoTrack setAppRegion:]`更新值
 */
- (void)setAppRegion:(nullable NSString *)appRegion;

/*! @abstract 语言
 @discussion 如果设置过，会保存值，直到下次改变或者清空
 @discussion 如果没有值，默认会读取 `[[NSLocale currentLocale] objectForKey:NSLocaleLanguageCode]`
 @discussion 发生变化时候请调用 `+[BDAutoTrack setAppLauguage:]`更新值
 */
- (void)setAppLauguage:(nullable NSString *)appLauguage;

/*! @abstract 设置注册成功回调
 @param registerFinishBlock id发生变化可能重新触发请求，请求回调。
 @discussion registerFinishBlock 会覆盖之前的初始化或者上一次设置的回调，如果为nil会清空回调
 @discussion block在初始化之前设置一次即可，每次拉取成功都会回调，请勿一直重复设置
 @discussion 推荐使用通知，请参考 <RangersAppLog/BDAutoTrackNotifications.h>注释，后续版本可能会移除此block
 */
- (void)setRegisterFinishBlock:(nullable BDAutoTrackRegisterFinishBlock)registerFinishBlock __attribute__((deprecated("请使用通知来监听回调")));

/*! @abstract 添加自定义上报信息
 @param customHeaderBlock 自定义上报信息
 @discussion customHeaderBlock 一次App启动设置一次即可；App重启需要重新设置，因为SDK不会保存上次设置的值；会覆盖之前的初始化的或者上一次设置的，如果为nil会清空回调
 @discussion block在初始化之前设置一次即可，每次都会回调，不会把block返回参数保存，而导致获取不到变化的值，请勿一直重复设置
 */
- (void)setCustomHeaderBlock:(nullable BDAutoTrackCustomHeaderBlock)customHeaderBlock;

/*! @abstract 日志上报
 @param event 事件名称，不能为nil或空字符串
 @param params 事件参数。可以为空或者nil，但是param如果非空，需要可序列化成json
 @discussion params 请参考文档中的日志格式要求，或者使用 [NSJSONSerialization isValidJSONObject:] 来检查params是否可序列化
 @result 是否成功，如果失败，则表示此日志不会被上报。原因是无法序列化。
 */
- (BOOL)eventV3:(NSString *)event params:(nullable NSDictionary *)params;

#pragma mark - ABTest

/*! @abstract 设置注册成功回调
 @param ABTestFinishBlock ABTest配置拉取，请求回调。回调在子线程，不是主线程
 @discussion ABTestFinishBlock 会覆盖之前的初始化或者上一次设置的回调，如果为nil会清空回调
 @discussion block在初始化之前设置一次即可，每次拉取成功都会回调，请勿一直重复设置。此回调会定时回调，并非一次性回调
 @discussion 推荐使用通知，请参考 <RangersAppLog/BDAutoTrackNotifications.h>注释，后续版本可能会移除此block
 */
- (void)setABTestFinishBlock:(nullable BDAutoTrackABTestFinishBlock)ABTestFinishBlock __attribute__((deprecated("请使用通知来监听回调")));

/*! @abstract 手动触发获取ABTest的配置值请求
 @discussion 手动触发请求
 */
- (void)startFetchABTestConfiguration;

/*! @abstract 获取ABTest的配置值
 @param key ABTest的key
 @param defaultValue 默认值，如果没有配置，或者未开启ABTest则返回默认值
 @discussion 未开启ABTest或者ABTest配置了默认值，都是返回默认值，因此建议监听ABTestFinishBlock来判断是否开启了ABTest
 @discussion 通过此接口读取会触发曝光统计
 @result 返回ABTest的配置值
 */
- (nullable id)ABTestConfigValueForKey:(NSString *)key defaultValue:(nullable id)defaultValue;

/*! @abstract 设置日志上报ABTest属性
 @param versions 设置完versions且ABTest开启的状态下，每条日志都会带上这个属性,对应ab_sdk_version字段
 @discussion 原来接口 +[BDAutoTrack setABServerVersions]
*/
- (void)setABSDKVersions:(nullable NSString *)versions;

/*! @abstract 获取ABTest相关配置
 @result 返回ABTest的abServerVersions值，此值就是之前setABSDKVersions:接口设置进来的
 @discussion 此值不需要设置到上报的event中，SDK会自动给每个event添加
 */
- (nullable NSString *)abSDKVersions;

/*! @abstract 获取ABTest相关配置
@result 返回ABTest的abVersions值
 @discussion 此值不需要设置到上报的event中，SDK会自动给每个event添加
 */
- (nullable NSString *)abVersions;

/*! @abstract 获取ABTest相关配置
 @result 返回ABTest的所有的abVersions值
 @discussion 此接口不会触发曝光，可以随意读取
 */
- (nullable NSString *)allAbVersions;

/*! @abstract 获取ABTest相关配置
 @result 返回ABTest的所有的Configs值
 @discussion 此接口不会触发曝光，可以随意读取。
 @discussion 如果正常为了做实验，请勿使用此接口，请使用-[BDAutoTrack ABTestConfigValueForKey:defaultValue:]接口
 */
- (nullable NSDictionary *)allABTestConfigs;

#pragma mark - private API

/*! @abstract 调用用户激活接口。一般情况下，请勿调用，除非知晓调用可能的问题。预留给内部CP接口使用
 */
- (void)activeUser;

@end

NS_ASSUME_NONNULL_END