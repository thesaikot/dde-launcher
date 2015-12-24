#include "dbuscontroller.h"
#include "dbusinterface/launcher_interface.h"
#include "dbusinterface/launchersettings_interface.h"
#include "dbusinterface/fileInfo_interface.h"
#include "dbusinterface/startmanager_interface.h"
#include "dbusinterface/displayinterface.h"
#include "dbusinterface/dock_interface.h"
#include "app/global.h"
#include "widgets/util.h"
#include "controller/menucontroller.h"
#include "dbusinterface/dbusclientmanager.h"
#include "dbusinterface/pinyin_interface.h"
#include <QDebug>
#include <QtCore>
#include <QtDBus>

inline bool compareByName(const QString& name1, const QString& name2)
{
    return name1 < name2;
}

bool appNameLessThan(const ItemInfo &info1, const ItemInfo &info2)
{
    return compareByName(info1.lowerPinyinName, info2.lowerPinyinName);
}

bool installTimeMoreThan(const ItemInfo &info1, const ItemInfo &info2)
{
    if (info1.installedTime > info2.installedTime) return true;
    if (info1.installedTime < info2.installedTime) return false;
    return compareByName(info1.lowerPinyinName, info2.lowerPinyinName);
}

bool useFrequencyMoreThan(const ItemInfo &info1, const ItemInfo &info2){
    if (info1.count > info2.count) return true;
    if (info1.count < info2.count) return false;
    return compareByName(info1.lowerPinyinName, info2.lowerPinyinName);
}

QSet<QString> DBusController::PreInstallAppKeys = {};

DBusController::DBusController(QObject *parent) : QObject(parent)
{
    m_launcherInterface = new LauncherInterface(Launcher_service, Launcher_path, QDBusConnection::sessionBus(), this);
    m_launcherSettingsInterface = new LauncherSettingsInterface(Launcher_service, Launcher_path, QDBusConnection::sessionBus(), this);
    m_fileInfoInterface = new FileInfoInterface(FileInfo_service, FileInfo_path, QDBusConnection::sessionBus(), this);
    m_startManagerInterface = new StartManagerInterface(StartManager_service, StartManager_path, QDBusConnection::sessionBus(), this);
    m_displayInterface = new DisplayInterface(this);
    m_dockInterface = new DockInterface(Dock_service, Dock_path, QDBusConnection::sessionBus(), this);
    m_dockClientManagerInterface = new DBusClientManager(this);
    m_pinyinInterface = new PinyinInterface(Pinyin_service, Pinyin_path, QDBusConnection::sessionBus(), this);
    m_menuController = new MenuController(this);
    initConnect();
}

void DBusController::init(){
    qDebug() << "get Launcher data";
    getAutoStartList();
    getCategoryInfoList();
    getInstalledTimeItems();
    getAllFrequencyItems();
    refreshUI();
}

void DBusController::refreshUI()
{
    int sortedMode= getSortMethod();
    int categoryMode = getCategoryDisplayMode();

    if (sortedMode == 0){
        emit signalManager->viewModeChanged(0);
    }else if (sortedMode == 1){
        emit signalManager->viewModeChanged(categoryMode + 1);
    }else if (sortedMode == 2 || sortedMode == 3){
        emit signalManager->viewModeChanged(sortedMode + 1);
    }
}

void DBusController::initConnect(){
    connect(m_launcherInterface, SIGNAL(ItemChanged(QString,ItemInfo,qlonglong)),
            this, SLOT(handleItemChanged(QString,ItemInfo,qlonglong)));
    connect(m_launcherInterface, SIGNAL(UninstallSuccess(QString)),
            m_menuController, SLOT(handleUninstallSuccess(QString)));
    connect(m_launcherInterface, SIGNAL(UninstallFailed(QString,QString)),
            m_menuController, SLOT(handleUninstallFail(QString,QString)));
    connect(m_launcherInterface, SIGNAL(SearchDone(QStringList)),
            this, SLOT(searchDone(QStringList)));
    connect(signalManager, SIGNAL(launcheRefreshed()), this, SLOT(init()));
    connect(signalManager, SIGNAL(search(QString)), m_launcherInterface, SLOT(Search(QString)));
    connect(signalManager, SIGNAL(itemDeleted(QString)), this, SLOT(updateAppTable(QString)));
    connect(signalManager, SIGNAL(sortedModeChanged(int)), this, SLOT(setSortMethod(int)));
    connect(signalManager, SIGNAL(categoryModeChanged(int)), this, SLOT(setCategoryDisplayMode(int)));
    connect(signalManager, SIGNAL(installTimeRefreshed()), this, SLOT(getInstalledTimeItems()));
    connect(signalManager, SIGNAL(frequencyRefreshed()), this, SLOT(getAllFrequencyItems()));
    connect(signalManager, SIGNAL(refreshInstallTimeFrequency()), this, SLOT(refreshInstallTimeFrequency()));
    connect(m_displayInterface, SIGNAL(PrimaryRectChanged()), signalManager, SIGNAL(screenGeometryChanged()));
    connect(m_displayInterface, SIGNAL(PrimaryChanged()), signalManager, SIGNAL(screenGeometryChanged()));
    connect(m_dockClientManagerInterface, SIGNAL(ActiveWindowChanged(uint)), signalManager, SIGNAL(activeWindowChanged(uint)));
}

void DBusController::updateAppTable(QString appKey){
    qDebug() << "updateAppTable" << appKey;
    emit signalManager->launcheRefreshed();
}

void DBusController::handleItemChanged(const QString &action, ItemInfo itemInfo,
                                       qlonglong categoryInfoId){
    Q_UNUSED(itemInfo)
    qDebug() << action << categoryInfoId;
    emit signalManager->launcheRefreshed();
    if (action == "created"){
        emit signalManager->iconRefreshed(itemInfo);
    }else if (action == "updated"){
        emit signalManager->newinstalllindicatorShowed(itemInfo.key);
    }else if (action == "deleted"){
    }else{
    }
}

LauncherInterface* DBusController::getLauncherInterface(){
    return m_launcherInterface;
}

StartManagerInterface* DBusController::getStartManagerInterface(){
    return m_startManagerInterface;
}

DisplayInterface* DBusController::getDisplayInterface(){
    return m_displayInterface;
}

DockInterface* DBusController::getDockInterface(){
    return m_dockInterface;
}

void DBusController::getAutoStartList(){
    m_autoStartList.clear();
    QDBusPendingReply<QStringList> reply = m_startManagerInterface->AutostartList();
    reply.waitForFinished();
    if (!reply.isError()){
        QStringList urlList = reply.argumentAt(0).toStringList();
        foreach (QString url, urlList) {
            m_autoStartList.append(QFileInfo(url).fileName());
        }
    }else{
        qCritical() << reply.error().message();
    }
}

void DBusController::getCategoryInfoList(){
    m_itemInfos.clear();
    m_categoryAppNameSortedInfoList.clear();
    QDBusPendingReply<CategoryInfoList> reply = m_launcherInterface->GetAllCategoryInfos();
    reply.waitForFinished();
    if (!reply.isError()){
        m_categoryInfoList = qdbus_cast<CategoryInfoList>(reply.argumentAt(0));
        foreach (CategoryInfo item, m_categoryInfoList) {
            if (item.key == "all" && item.id == -1){
                foreach (QString appKey, item.items){
                    ItemInfo itemInfo= getItemInfo(appKey);
                    if (m_autoStartList.contains(QFileInfo(itemInfo.url).fileName())){
                        itemInfo.isAutoStart = true;
                    }
                    m_itemInfos.insert(appKey, itemInfo);
                }
                emit signalManager->itemInfosChanged(m_itemInfos);
            }
        }

        convertNameToPinyin();

        sortedByAppName(m_itemInfos.values());

        bool isPreIsntallExists = isPreInsallAppsPathExists();
        qDebug() << getPreInstallAppsPath() << isPreIsntallExists;
        if (isPreIsntallExists){
            loadPreInstallApps();
        }
        foreach (CategoryInfo item, m_categoryInfoList) {
            if (item.key != "all" && item.id != -1){
                QStringList appKeys;
                foreach (ItemInfo itemInfo, m_appNameSortedList) {
                    if (item.items.contains(itemInfo.key)){
                        appKeys.append(itemInfo.key);
                        if (itemInfo.count == 0 && !isPreIsntallExists){
                            PreInstallAppKeys.insert(itemInfo.key);
                        }
                    }
                }
                item.items = appKeys;

//                qDebug() << appKeys;
            }
            m_categoryAppNameSortedInfoList.append(item);
        }
        qDebug() << "m_categoryAppNameSortedInfoList"<< m_categoryAppNameSortedInfoList.length();
        emit signalManager->categoryInfosChanged(m_categoryAppNameSortedInfoList);
        if (!isPreIsntallExists){
            savePreInstallApps();
        }
    }else{
        qCritical() << reply.error().message();
    }
}

void DBusController::convertNameToPinyin(){
    QStringList names;
    foreach(QString appKey, m_itemInfos.keys()){
        names.append(m_itemInfos.value(appKey).name);
    }

    QDBusPendingReply<QString> reply = m_pinyinInterface->QueryList(names);
    reply.waitForFinished();
    if (!reply.isError()){
        QString result = reply.argumentAt(0).toString();
        QJsonObject obj = QJsonObject::fromVariantMap(QJsonDocument::fromJson(result.toLocal8Bit()).toVariant().toMap());
        foreach(QString appKey, m_itemInfos.keys()){
            QString name = m_itemInfos.value(appKey).name;
            if (obj.contains(name)){
                  QList<QVariant> pinyins  = obj.value(name).toVariant().toList();
                  if (pinyins.length() > 0){
                      m_itemInfos[appKey].pinyinName = pinyins.at(0).toString();
                      m_itemInfos[appKey].lowerPinyinName = m_itemInfos[appKey].pinyinName.toLower();
                  }
            }
        }
        m_pinyinEnglishInfos = sortPingyinEnglish(m_itemInfos.values());
    }else{
        qCritical() << reply.error().message();
    }
}


void DBusController::sortedByAppName(QList<ItemInfo> infos){
    Q_UNUSED(infos)
    QList<ItemInfo> pinyinInfos = m_pinyinEnglishInfos[0];
    QList<ItemInfo> englishInfos = m_pinyinEnglishInfos[1];
    std::sort(pinyinInfos.begin(), pinyinInfos.end(), appNameLessThan);
    std::sort(englishInfos.begin(), englishInfos.end(), appNameLessThan);
    m_appNameSortedList.clear();
    m_appNameSortedList = pinyinInfos + englishInfos;
    emit signalManager->appNameItemInfoListChanged(m_appNameSortedList);
}

void DBusController::sortedByInstallTime(QList<ItemInfo> infos){
    std::sort(infos.begin(), infos.end(), installTimeMoreThan);
    m_installTimeSortedList.clear();
    m_installTimeSortedList = infos;
    emit signalManager->installTimeItemInfoListChanged(m_installTimeSortedList);
}

void DBusController::sortedByFrequency(QList<ItemInfo> infos){
    std::sort(infos.begin(), infos.end(), useFrequencyMoreThan);
    m_useFrequencySortedList.clear();
    m_useFrequencySortedList = infos;
//    for(int i=0; i< m_useFrequencySortedList.count(); i++){
//        qDebug() << m_useFrequencySortedList.at(i).name << m_useFrequencySortedList.at(i).count;
//    }
    emit signalManager->useFrequencyItemInfoListChanged(m_useFrequencySortedList);
}

QList<QList<ItemInfo>>  DBusController::sortPingyinEnglish(QList<ItemInfo> infos){
    QList<QList<ItemInfo>>  result;
    QList<ItemInfo> pinyinInfos;
    QList<ItemInfo> englishInfos;
    foreach (ItemInfo info, infos) {
        if (info.pinyinName.length() > 0 && info.name.length() > 0){
            if (info.pinyinName.at(0) == info.name.at(0)){
                englishInfos.append(info);
            }else{
                pinyinInfos.append(info);
            }
        }
    }
    result.append(pinyinInfos);
    result.append(englishInfos);
    return result;
}

void DBusController::savePreInstallApps()
{
    QStringList keys;
    foreach (QString key, PreInstallAppKeys) {
        keys.append(key);
    }
    QString path = getPreInstallAppsPath();
    qDebug() << path;
    QString appKeysString = QString(QJsonDocument(QJsonArray::fromStringList(keys)).toJson());
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    file.write(appKeysString.toLocal8Bit());
    file.close();
}

void DBusController::loadPreInstallApps()
{
    QString path = getPreInstallAppsPath();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QJsonParseError* error = new QJsonParseError();
    QStringList preInstalledAppKeys = QJsonDocument::fromJson(QString(file.readAll()).toStdString().c_str(), error).toVariant().toStringList();
    PreInstallAppKeys = preInstalledAppKeys.toSet();
    file.close();
}

QString DBusController::getPreInstallAppsPath()
{
    QString configPath = QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).at(0);
    configPath = QString("%1/%2/%3").arg(configPath, qApp->organizationName(), qApp->applicationName());
    if (!QDir(configPath).exists()){
        QDir(configPath).mkdir(configPath);
    }
    QString path = joinPath(configPath, "preInstallApps.json");
    return path;
}

bool DBusController::isPreInsallAppsPathExists()
{
    return QFile(getPreInstallAppsPath()).exists();
}

void DBusController::getAllFrequencyItems(){
    m_useFrequencySortedList.clear();
    QDBusPendingReply<AppFrequencyInfoList> reply = m_launcherInterface->GetAllFrequency();
    reply.waitForFinished();
    if (!reply.isError()){
        m_appFrequencyInfoList = qdbus_cast<AppFrequencyInfoList>(reply.argumentAt(0));
        for(int i=0; i< m_appFrequencyInfoList.count(); i++){
            QString key = m_appFrequencyInfoList.at(i).key;
            if (m_itemInfos.contains(key)){
//                qDebug() << key << m_appFrequencyInfoList[i].count;
                m_itemInfos[key].count = m_appFrequencyInfoList[i].count;
            }
        }
        emit signalManager->itemInfosChanged(m_itemInfos);
        sortedByFrequency(m_itemInfos.values());
    }else{
        qCritical() << reply.error().message();
    }


}

void DBusController::getInstalledTimeItems(){
    m_installTimeSortedList.clear();
    QDBusPendingReply<AppInstalledTimeInfoList> reply = m_launcherInterface->GetAllTimeInstalled();
    reply.waitForFinished();
    if (!reply.isError()){
        m_appInstalledTimeInfoList = qdbus_cast<AppInstalledTimeInfoList>(reply.argumentAt(0));
        for(int i=0; i< m_appInstalledTimeInfoList.count(); i++){
            QString key = m_appInstalledTimeInfoList.at(i).key;
            if (m_itemInfos.contains(key)){
                m_itemInfos[key].installedTime = m_appInstalledTimeInfoList.at(i).installedTime;
            }
        }
        sortedByInstallTime(m_itemInfos.values());
    }else{
        qCritical() << reply.error().message();
    }
}


ItemInfo DBusController::getItemInfo(QString appKey){
    QDBusPendingReply<ItemInfo> reply = m_launcherInterface->GetItemInfo(appKey);
    ItemInfo  itemInfo;
    reply.waitForFinished();
    if (!reply.isError()){
        itemInfo = qdbus_cast<ItemInfo>(reply.argumentAt(0));
        return itemInfo;
    }else{
        qCritical() << reply.error().message();
        return itemInfo;
    }
}

ItemInfo DBusController::getLocalItemInfo(QString appKey){
    if (m_itemInfos.contains(appKey)){
        return m_itemInfos.value(appKey);
    }
    return ItemInfo();
}

int DBusController::getCategoryDisplayMode(){
    QDBusPendingReply<qlonglong> reply = m_launcherSettingsInterface->GetCategoryDisplayMode();
    reply.waitForFinished();
    if (!reply.isError()){
        qlonglong mode = reply.argumentAt(0).toLongLong();
        return mode;
    }else{
        qCritical() << reply.error().message();
        return 0;
    }
}

void DBusController::setCategoryDisplayMode(int mode){
    QDBusPendingReply<qlonglong> reply = m_launcherSettingsInterface->SetCategoryDisplayMode(mode);
}


int DBusController::getSortMethod(){
    QDBusPendingReply<qlonglong> reply = m_launcherSettingsInterface->GetSortMethod();
    reply.waitForFinished();
    if (!reply.isError()){
        qlonglong mode = reply.argumentAt(0).toLongLong();
        return mode;
    }else{
        qCritical() << reply.error().message();
        return 0;
    }
}

void DBusController::setSortMethod(int mode){
    QDBusPendingReply<qlonglong> reply = m_launcherSettingsInterface->SetSortMethod(mode);
}

void DBusController::refreshInstallTimeFrequency(){
    int mode = getSortMethod();
    qDebug() << "refreshInstallTimeFrequency" << mode;
    if (mode == 2){
        emit signalManager->installTimeRefreshed();
    }else if (mode == 3){
        emit signalManager->frequencyRefreshed();
    }
    refreshUI();
}

void DBusController::searchDone(QStringList appKeys){
    m_searchList.clear();
    foreach (const ItemInfo& info, m_appNameSortedList) {
        if (appKeys.contains(info.key)){
            m_searchList.append(info);
        }
    }
    emit signalManager->searchItemInfoListChanged(m_searchList);
}

DBusController::~DBusController()
{

}

