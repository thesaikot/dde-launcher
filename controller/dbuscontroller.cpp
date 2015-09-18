#include "dbuscontroller.h"
#include "dbusinterface/launcher_interface.h"
#include "dbusinterface/launchersettings_interface.h"
#include "dbusinterface/fileInfo_interface.h"
#include "dbusinterface/startmanager_interface.h"
#include "Logger.h"
#include "app/global.h"
#include "controller/menucontroller.h"

#include <QtCore>
#include <QtDBus>

bool appNameLessThan(const ItemInfo &info1, const ItemInfo &info2)
{
    return info1.name < info2.name;
}

bool installTimeLessThan(const ItemInfo &info1, const ItemInfo &info2)
{
    return info1.intsalledTime < info2.intsalledTime;
}

bool useFrequencyMoreThan(const AppFrequencyInfo &info1, const AppFrequencyInfo &info2){
    return info1.count > info2.count;
}

DBusController::DBusController(QObject *parent) : QObject(parent)
{
    m_launcherInterface = new LauncherInterface(Launcher_service, Launcher_path, QDBusConnection::sessionBus(), this);
    m_launcherSettingsInterface = new LauncherSettingsInterface(Launcher_service, Launcher_path, QDBusConnection::sessionBus(), this);
    m_fileInfoInterface = new FileInfoInterface(FileInfo_service, FileInfo_path, QDBusConnection::sessionBus(), this);
    m_startManagerInterface = new StartManagerInterface(StartManager_service, StartManager_path, QDBusConnection::sessionBus(), this);
    m_menuController = new MenuController(this);
    initConnect();
}

void DBusController::init(){
    LOG_INFO() << "get Launcher data";
    getAutoStartList();
    getCategoryInfoList();
    getInstalledTimeItems();
    getAllFrequencyItems();
    int sortedMode= getSortMethod();
    int categoryMode = getCategoryDisplayMode();
    LOG_INFO() << sortedMode << categoryMode << "========";
    if (sortedMode == 0){
        emit signalManager->viewModeChanged(0);
    }else if (sortedMode == 1){
        emit signalManager->viewModeChanged(categoryMode + 1);
    }else if (sortedMode == 2 || sortedMode == 3){
        emit signalManager->viewModeChanged(sortedMode + 1);
    }
}

void DBusController::initConnect(){
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
}

void DBusController::updateAppTable(QString appKey){
    LOG_INFO() << "updateAppTable" << appKey;
    init();
}

LauncherInterface* DBusController::getLauncherInterface(){
    return m_launcherInterface;
}

StartManagerInterface* DBusController::getStartManagerInterface(){
    return m_startManagerInterface;
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
        LOG_ERROR() << reply.error().message();
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

        sortedByAppName(m_itemInfos.values());
        foreach (CategoryInfo item, m_categoryInfoList) {
            if (item.key != "all" && item.id != -1){
                QStringList appKeys;
                foreach (ItemInfo itemInfo, m_appNameSortedList) {
                    if (item.items.contains(itemInfo.key)){
                        appKeys.append(itemInfo.key);
                    }
                }
                item.items = appKeys;
            }
            m_categoryAppNameSortedInfoList.append(item);
        }
        emit signalManager->categoryInfosChanged(m_categoryAppNameSortedInfoList);
    }else{
        LOG_ERROR() << reply.error().message();
    }
}


void DBusController::sortedByAppName(QList<ItemInfo> infos){
    std::sort(infos.begin(), infos.end(), appNameLessThan);
    m_appNameSortedList.clear();
    m_appNameSortedList = infos;
    emit signalManager->appNameItemInfoListChanged(m_appNameSortedList);
}

void DBusController::sortedByInstallTime(QList<ItemInfo> infos){
    std::sort(infos.begin(), infos.end(), installTimeLessThan);
    m_installTimeSortedList.clear();
    m_installTimeSortedList = infos;
    emit signalManager->installTimeItemInfoListChanged(m_installTimeSortedList);
}

void DBusController::getAllFrequencyItems(){
    m_useFrequencySortedList.clear();
    QDBusPendingReply<AppFrequencyInfoList> reply = m_launcherInterface->GetAllFrequency();
    reply.waitForFinished();
    if (!reply.isError()){
        m_appFrequencyInfoList = qdbus_cast<AppFrequencyInfoList>(reply.argumentAt(0));
        std::sort(m_appFrequencyInfoList.begin(), m_appFrequencyInfoList.end(), useFrequencyMoreThan);
        for(int i=0; i< m_appFrequencyInfoList.count(); i++){
            QString key = m_appFrequencyInfoList.at(i).key;
            if (m_itemInfos.contains(key)){
                m_useFrequencySortedList.append(m_itemInfos.value(key));
            }
        }
        emit signalManager->useFrequencyItemInfoListChanged(m_useFrequencySortedList);
    }else{
        LOG_ERROR() << reply.error().message();
    }


}

void DBusController::getInstalledTimeItems(){
    sortedByInstallTime(m_itemInfos.values());
}


ItemInfo DBusController::getItemInfo(QString appKey){
    QDBusPendingReply<ItemInfo> reply = m_launcherInterface->GetItemInfo(appKey);
    ItemInfo  itemInfo;
    reply.waitForFinished();
    if (!reply.isError()){
        itemInfo = qdbus_cast<ItemInfo>(reply.argumentAt(0));
        return itemInfo;
    }else{
        LOG_ERROR() << reply.error().message();
        return itemInfo;
    }
}

int DBusController::getCategoryDisplayMode(){
    QDBusPendingReply<qlonglong> reply = m_launcherSettingsInterface->GetCategoryDisplayMode();
    reply.waitForFinished();
    if (!reply.isError()){
        qlonglong mode = reply.argumentAt(0).toLongLong();
        return mode;
    }else{
        LOG_ERROR() << reply.error().message();
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
        LOG_ERROR() << reply.error().message();
        return 0;
    }
}

void DBusController::setSortMethod(int mode){
    QDBusPendingReply<qlonglong> reply = m_launcherSettingsInterface->SetSortMethod(mode);
}

void DBusController::searchDone(QStringList appKeys){
    m_searchList.clear();
    foreach(QString appKey, appKeys){
        m_searchList.append(m_itemInfos.value(appKey));
    }
    emit signalManager->showSearchResult();
    emit signalManager->searchItemInfoListChanged(m_searchList);
}

DBusController::~DBusController()
{

}
