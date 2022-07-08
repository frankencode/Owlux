/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see Owlux/LICENSE-gpl-3.0).
 *
 */

#include <owlux/YeelightResult>
#include <owlux/YeelightProtocolError>
#include <cc/MetaObject>
#include <cc/json>

namespace cc::owlux {

struct YeelightResult::State: public YeelightResponse::State
{
    State() = default;

    explicit State(const MetaObject &message):
        message_{message}
    {
        YEELIGHT_EXPECT(message.contains("id"));

        Variant idValue = message("id");
        YEELIGHT_EXPECT(idValue.is<long>());

        commandId_ = message("id").to<long>();

        bool isOk = message.contains("result");
        bool isError = message.contains("error");
        YEELIGHT_EXPECT(isOk + isError == 1);

        isError_ = isError;

        if (isError_) {
            Variant errorValue = message("error");
            YEELIGHT_EXPECT(errorValue.is<MetaObject>());
            errorDetails_ = errorValue.to<MetaObject>().members();
        }
        else {
            Variant resultValue = message("result");
            YEELIGHT_EXPECT(resultValue.is<List<Variant>>());
            values_ = resultValue.to<List<Variant>>();
        }
    }

    int errorCode() const
    {
        Variant value = errorDetails_.value("code");
        return value.is<long>() ? value.to<long>() : -1;
    }

    String errorMessage() const
    {
        Variant value = errorDetails_.value("message");
        return value.is<String>() ? value.to<String>() : String{};
    }

    String toString() const override
    {
        return jsonStringify(message_);
    }

    MetaObject message_;

    long commandId_ { -1 };
    bool isError_ { false };

    Map<String, Variant> errorDetails_;
    List<Variant> values_;
};

bool YeelightResult::recognise(const MetaObject &message)
{
    return message.contains("result") || message.contains("error");
}

YeelightResult::YeelightResult(const MetaObject &message):
    YeelightResponse{new State{message}}
{}

YeelightResult::YeelightResult(const YeelightResponse &response)
{
    if (response.is<YeelightResult>()) {
        *this = response.as<YeelightResult>();
    }
}

long YeelightResult::commandId() const
{
    return me().commandId_;
}

bool YeelightResult::isError() const
{
    return me().isError_;
}

const Map<String, Variant> &YeelightResult::errorDetails() const
{
    return me().errorDetails_;
}

int YeelightResult::errorCode() const
{
    return me().errorCode();
}

String YeelightResult::errorMessage() const
{
    return me().errorMessage();
}

bool YeelightResult::isOk() const
{
    return !me().isError_;
}

const List<Variant> &YeelightResult::values() const
{
    return me().values_;
}

const YeelightResult::State &YeelightResult::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::owlux
