#ifndef HAVOCCLIENT_HAVOC_H
#define HAVOCCLIENT_HAVOC_H

#include <Common.h>
#include <Events.h>

class HcMainWindow;

/* Havoc include */
#include <core/HcHelper.h>
#include <core/HcEventWorker.h>
#include <core/HcHeartbeatWorker.h>

#include <ui/HcPageAgent.h>
#include <ui/HcPageListener.h>
#include <ui/HcDialogBuilder.h>
#include <ui/HcPageScript.h>

#include <ui/HcListenerDialog.h>
#include <ui/HcMainWindow.h>
#include <ui/HcConnectDialog.h>
#include <ui/HcLineEdit.h>
#include <ui/HcTheme.h>

#include <api/Engine.h>

#define HAVOC_VERSION  "1.0"
#define HAVOC_CODENAME "King Of The Damned"

class HavocClient : public QWidget {

    using toml_t = toml::basic_value<toml::discard_comments, std::unordered_map, std::vector>;

    struct NamedObject {
        std::string  name;
        py11::object object;
    };

    //
    // current connection info
    //
    struct {
        std::string Name;
        std::string Host;
        std::string Port;
        std::string User;
        std::string Pass;

        //
        // login token to interact
        // with the api endpoints
        //
        std::string Token;
    } Profile;

    struct {
        QThread*     Thread;
        HcEventWorker* Worker;
    } Events;

    struct {
        QThread*           Thread;
        HcHeartbeatWorker* Worker;
    } Heartbeat;

    std::vector<json>         listeners  = {};
    std::vector<NamedObject>  protocols  = {};
    std::vector<NamedObject>  builders   = {};
    std::vector<NamedObject>  agents     = {};
    std::vector<NamedObject>  callbacks  = {};

public:
    HcMainWindow* Gui    = nullptr;
    toml_t        Config = {};
    HcTheme       Theme;

    struct {
        QThread*    Thread;
        HcPyEngine* Engine;
    } Python;

    /* havoc client constructor and destructor */
    explicit HavocClient();
    ~HavocClient() override;

    /* client entrypoint */
    auto Main(
        int    argc,
        char** argv
    ) -> void;

    /* exit application and free resources */
    auto Exit() -> void;

    /* get server address */
    auto Server() const -> std::string;

    /* get server connection token */
    auto Token() const -> std::string;

    static auto StyleSheet() -> QByteArray;

    auto SetupThreads() -> void;

    auto SetupDirectory() -> bool;

    //
    // Callbacks
    //

    auto Callbacks() -> std::vector<std::string>;

    auto AddCallbackObject(
        const std::string&  uuid,
        const py11::object& callback
    ) -> void;

    auto RemoveCallbackObject(
        const std::string& uuid
    ) -> void;

    auto CallbackObject(
        const std::string& uuid
    ) -> std::optional<py11::object>;

    //
    // Listeners
    //

    auto Listeners() -> std::vector<std::string>;

    auto ListenerObject(
        const std::string& name
    ) -> std::optional<json>;

    auto AddListener(
        const json& listener
    ) -> void;

    //
    // Protocols
    //

    auto Protocols() -> std::vector<std::string>;

    auto AddProtocol(
        const std::string&  name,
        const py11::object& listener
    ) -> void;

    auto ProtocolObject(
        const std::string& name
    ) -> std::optional<py11::object>;

    //
    // Payload Builder
    //
    auto AddBuilder(
        const std::string & name,
        const py11::object& builder
    ) -> void;

    auto BuilderObject(
        const std::string& name
    ) -> std::optional<py11::object>;

    auto Builders() -> std::vector<std::string>;

    //
    // Agent api
    //
    auto Agent(
        const std::string& uuid
    ) const -> std::optional<HcAgent*>;

    auto Agents() -> std::vector<HcAgent*>;

    auto AddAgentObject(
        const std::string&  type,
        const py11::object& object
    ) -> void;

    auto AgentObject(
        const std::string& type
    ) -> std::optional<py11::object>;

    //
    // Server Api
    //

    /* send request to api endpoint */
    auto ApiSend(
        const std::string& endpoint,
        const json&        body,
        const bool         keep_alive = false
    ) const -> httplib::Result;

Q_SIGNALS:
    /* signals */
    auto eventHandle(
        const QByteArray& request
    ) -> void;

    auto eventDispatch(
        const json& event
    ) -> void;

    auto eventClosed() -> void;
};

/* a global Havoc app instance */
extern HavocClient* Havoc;

auto HttpErrorToString(
    const httplib::Error& error
) -> std::optional<std::string>;

#endif //HAVOCCLIENT_HAVOC_H
