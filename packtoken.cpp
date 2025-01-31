#include <sstream>
#include <string>
#include <iostream>

#include "rpnbuilder.h"
#include "packtoken.h"
#include "reftoken.h"

using cparse::Function;
using cparse::PackToken;
using cparse::STuple;
using cparse::Token;
using cparse::TokenList;
using cparse::TokenMap;
using cparse::Tuple;

namespace {
    using namespace cparse;

    PackToken &noneToken()
    {
        static PackToken none = PackToken(TokenNone());
        return none;
    }

    PackToken &errorToken()
    {
        static PackToken none = PackToken(TokenError());
        return none;
    }

    PackToken &rejectToken()
    {
        static PackToken none = PackToken(TokenReject());
        return none;
    }
}

const PackToken &PackToken::None()
{
    return noneToken();
}

const PackToken &PackToken::Error()
{
    return errorToken();
}

PackToken PackToken::Error(const QString &cause)
{
    return TokenError(cause);
}

const PackToken &PackToken::Reject()
{
    return rejectToken();
}

PackToken::ToStringFunc &PackToken::str_custom()
{
    static ToStringFunc func = nullptr;
    return func;
}

PackToken::PackToken(const Token &t) : m_base(t.clone()) { }
PackToken::PackToken() : m_base(new TokenNone()) { }
PackToken::PackToken(const TokenMap &map) : m_base(new TokenMap(map)) { }
PackToken::PackToken(const TokenList &list) : m_base(new TokenList(list)) { }

PackToken::~PackToken()
{
    delete m_base;
}

PackToken::PackToken(PackToken &&t) noexcept : m_base(t.m_base)
{
    t.m_base = nullptr;
}

PackToken::PackToken(const PackToken &t) : m_base(t.m_base->clone()) { }

PackToken &PackToken::operator=(const PackToken &t)
{
    if (this == &t) {
        return *this;
    }

    delete m_base;
    m_base = t.m_base->clone();
    return *this;
}

bool PackToken::operator==(const PackToken &token) const
{
    if (NUM & token.m_base->m_type & m_base->m_type) {
        return token.asReal() == asReal();
    }

    if (token.m_base->m_type != m_base->m_type) {
        return false;
    }

    // Compare strings to simplify code
    return token.str() == str();
}

bool PackToken::operator!=(const PackToken &token) const
{
    return !(*this == token);
}

Token *PackToken::operator->() const
{
    return m_base;
}

QDebug cparse::operator<<(QDebug os, const PackToken &t)
{
    return os << t.str();
}

std::ostream &cparse::operator<<(std::ostream &os, const PackToken &t)
{
    return os << t.str().toStdString().c_str();
}

PackToken &PackToken::operator[](const QString &key)
{
    if (m_base->m_type != MAP) {
        Q_ASSERT(false);
        return errorToken();
    }

    return (*static_cast<TokenMap *>(m_base))[key];
}

Token *PackToken::token()
{
    return m_base;
}

const Token *PackToken::token() const
{
    return m_base;
}

TokenType PackToken::type() const
{
    return m_base->m_type;
}

bool PackToken::isError() const
{
    return m_base->m_type == TokenType::ERROR;
}

bool PackToken::isEmpty() const
{
    switch (m_base->m_type) {
    case VAR:
    case STR:
        return static_cast<TokenTyped<QString> *>(m_base)->m_val.isEmpty();
    case MAP:
        return static_cast<TokenMap *>(m_base)->map().empty();
    case LIST:
        return static_cast<TokenList *>(m_base)->list().empty();
    case TUPLE:
        return static_cast<Tuple *>(m_base)->list().empty();
    case STUPLE:
        return static_cast<STuple *>(m_base)->list().empty();
    case NONE:
        return true;
    default:
        break;
    }
    return false;
}

const PackToken &PackToken::operator[](const QString &key) const
{
    if (m_base->m_type != MAP) {
        Q_ASSERT(false);
        return errorToken();
    }

    return (*static_cast<TokenMap *>(m_base))[key];
}
PackToken &PackToken::operator[](const char *key)
{
    if (m_base->m_type != MAP) {
        Q_ASSERT(false);
        return errorToken();
    }

    return (*static_cast<TokenMap *>(m_base))[key];
}

const PackToken &PackToken::operator[](const char *key) const
{
    if (m_base->m_type != MAP) {
        Q_ASSERT(false);
        return errorToken();
    }

    return (*static_cast<TokenMap *>(m_base))[key];
}

bool PackToken::canConvertToBool() const
{
    return m_base->canConvertTo(BOOL);
}

bool PackToken::asBool() const
{
    return m_base->asBool();
}

bool PackToken::canConvertToReal() const
{
    return canConvertTo(REAL);
}

qreal PackToken::asReal() const
{
    return m_base->asReal();
}

bool PackToken::canConvertToInt() const
{
    return canConvertTo(INT);
}

qint64 PackToken::asInt() const
{
    return m_base->asInt();
}

bool PackToken::canConvertToString() const
{
    return canConvertTo(STR);
}

QString PackToken::asString() const
{
    return m_base->asString();
}

bool PackToken::canConvertToMap() const
{
    return canConvertTo(MAP);
}
bool PackToken::canConvertToList() const
{
    return canConvertTo(LIST);
}
bool PackToken::canConvertToTuple() const
{
    return canConvertTo(TUPLE);
}
bool PackToken::canConvertToSTuple() const
{
    return canConvertTo(STUPLE);
}
bool PackToken::canConvertToFunc() const
{
    return canConvertTo(FUNC);
}

bool PackToken::canConvertTo(TokenType type) const
{
    return m_base->canConvertTo(type);
}

bool cparse::Token::canConvertTo(TokenType type) const
{
    if (m_type == type) {
        return true;
    }

    switch (type) {
    case BOOL: {
        switch (m_type) {
        case REAL:
        case INT:
        case BOOL:
        case STR:
        case MAP:
        case FUNC:
        case NONE:
        case TUPLE:
        case STUPLE:
            return true;

        default:
            break;
        }

        return false;
    }

    case REAL:
    case INT: {
        switch (m_type) {
        case REAL:
        case INT:
        case BOOL:
            return true;

        default:
            break;
        }

        return false;
    }

    case STR: {
        switch (m_type) {
        case STR:
        case VAR:
        case OP:
            return true;

        default:
            break;
        }

        return false;
    }

    default:
        break;
    }

    return false;
}

bool cparse::Token::asBool() const
{
    switch (m_type) {
    case REAL:
        return static_cast<const TokenTyped<qreal> *>(this)->m_val != 0;

    case INT:
        return static_cast<const TokenTyped<qint64> *>(this)->m_val != 0;

    case BOOL:
        return static_cast<const TokenTyped<uint8_t> *>(this)->m_val != 0;

    case STR:
        return !static_cast<const TokenTyped<QString> *>(this)->m_val.isEmpty();

    case MAP:
    case FUNC:
        return true;

    case NONE:
        return false;

    case ERROR:
        Q_ASSERT_X(false, "PackToken::asReal", "cannot convert an error to type!");

    case TUPLE:
    case STUPLE:
        return !static_cast<const Tuple *>(this)->list().empty();

    default:
        Q_ASSERT_X(false, "PackToken::asBool", "internal type can not be cast to boolean!");
    }

    return false;
}

qreal cparse::Token::asReal() const
{
    switch (m_type) {
    case REAL:
        return static_cast<const TokenTyped<qreal> *>(this)->m_val;

    case INT:
        return static_cast<qreal>(static_cast<const TokenTyped<qint64> *>(this)->m_val);

    case BOOL:
        return static_cast<const TokenTyped<uint8_t> *>(this)->m_val;

    case ERROR:
        Q_ASSERT_X(false, "PackToken::asReal", "cannot convert an error to type!");

    default: {
        if (!(m_type & NUM)) {
            Q_ASSERT_X(false, "PackToken::asReal", "internal type is not a number!");
        } else {
            Q_ASSERT_X(false, "PackToken::asReal", "internal type is an unsupported number type!");
        }
    }
    }

    return 0.0;
}

qint64 cparse::Token::asInt() const
{
    switch (m_type) {
    case REAL:
        return static_cast<qint64>(static_cast<const TokenTyped<qreal> *>(this)->m_val);

    case INT:
        return static_cast<const TokenTyped<qint64> *>(this)->m_val;

    case BOOL:
        return static_cast<const TokenTyped<uint8_t> *>(this)->m_val;

    case ERROR:
        Q_ASSERT_X(false, "PackToken::asReal", "cannot convert an error to type!");

    default:
        if (!(m_type & NUM)) {
            Q_ASSERT_X(false, "PackToken::asInt", "internal type is not a number!");
        } else {
            Q_ASSERT_X(false, "PackToken::asInt", "internal type is an unsupported number type!");
        }
    }

    return 0;
}

QString cparse::Token::asString() const
{
    if (m_type == STR || m_type == VAR) {
        return static_cast<const TokenTyped<QString> *>(this)->m_val;
    }

    Q_ASSERT_X(false, "Token::asString", "internal type is not a string!");
    return {};
}

TokenMap &PackToken::asMap() const
{
    if (m_base->m_type != MAP) {
        Q_ASSERT(false);
        static TokenMap nonType;
        return nonType;
    }

    return *static_cast<TokenMap *>(m_base);
}

TokenList &PackToken::asList() const
{
    if (m_base->m_type != LIST) {
        Q_ASSERT(false);
        static TokenList nonType;
        return nonType;
    }

    return *static_cast<TokenList *>(m_base);
}

Tuple &PackToken::asTuple() const
{
    if (m_base->m_type != TUPLE) {
        Q_ASSERT(false);
        static Tuple nonType;
        return nonType;
    }

    return *static_cast<Tuple *>(m_base);
}

STuple &PackToken::asSTuple() const
{
    if (m_base->m_type != STUPLE) {
        Q_ASSERT(false);
        static STuple nonType;
        return nonType;
    }

    return *static_cast<STuple *>(m_base);
}

Function *PackToken::asFunc() const
{
    if (m_base->m_type != FUNC) {
        Q_ASSERT(false);
        static Function *nonType = nullptr;
        return nonType;
    }

    return static_cast<Function *>(m_base);
}

QString PackToken::str(uint32_t nest) const
{
    return PackToken::str(m_base, nest);
}

QString PackToken::str(const Token *base, uint32_t nest)
{
    QString ss;
    TokenMap::MapType *tmap;
    TokenMap::MapType::iterator m_it;

    TokenList::ListType *tlist;
    TokenList::ListType::iterator l_it;
    const Function *func;
    bool first, boolval;
    QString name;

    if (!base) {
        return "undefined";
    }

    if (base->m_type & REF) {
        base = static_cast<const RefToken *>(base)->resolve(nullptr, nullptr);
        name = static_cast<const RefToken *>(base)->m_key.str();
    }

    /* * * * * Check for a user defined functions: * * * * */

    if (PackToken::str_custom()) {
        auto result = PackToken::str_custom()(base, nest);

        if (!result.isEmpty()) {
            return result;
        }
    }

    /* * * * * Stringify the token: * * * * */

    switch (base->m_type) {
    case NONE:
        return "none";

    case ERROR: {
        const auto cause = static_cast<const TokenError *>(base)->cause();
        return "error: " + (cause.isEmpty() ? "unknown" : cause);
    }

    case UNARY:
        return "unarytoken";

    case OP:
        return static_cast<const TokenTyped<QString> *>(base)->m_val;

    case VAR:
        return static_cast<const TokenTyped<QString> *>(base)->m_val;

    case REAL:
        ss += QString::number(static_cast<const TokenTyped<qreal> *>(base)->m_val);
        return ss;

    case INT:
        ss += QString::number(static_cast<const TokenTyped<qint64> *>(base)->m_val);
        return ss;

    case BOOL:
        boolval = static_cast<const TokenTyped<uint8_t> *>(base)->m_val;
        return boolval ? "true" : "false";

    case STR:
        return "\"" + static_cast<const TokenTyped<QString> *>(base)->m_val + "\"";

    case FUNC:
        func = static_cast<const Function *>(base);

        if (!func->name().isEmpty()) {
            return "[function: " + func->name() + "]";
        }

        if (!name.isEmpty()) {
            return "[function: " + name + "]";
        }

        return "[function]";

    case TUPLE:
    case STUPLE:
        if (nest == 0) {
            return "[tuple]";
        }

        ss += "(";
        first = true;

        for (const PackToken &token : static_cast<const Tuple *>(base)->list()) {
            if (!first) {
                ss += ", ";
            } else {
                first = false;
            }

            ss += str(token.token(), nest - 1);
        }

        if (first) {
            // Its an empty tuple:
            // Add a `,` to make it different than ():
            ss += ",)";
        } else {
            ss += ")";
        }

        return ss;

    case MAP:
        if (nest == 0) {
            return "[map]";
        }

        tmap = &(static_cast<const TokenMap *>(base)->map());

        if (tmap->empty()) {
            return "{}";
        }

        ss += "{";

        for (m_it = tmap->begin(); m_it != tmap->end(); ++m_it) {
            ss += (m_it == tmap->begin() ? "" : ",");
            ss += " \"" + m_it->first + "\": " + m_it->second.str(nest - 1);
        }

        ss += " }";
        return ss;

    case LIST:
        if (nest == 0) {
            return "[list]";
        }

        tlist = &(static_cast<const TokenList *>(base)->list());

        if (tlist->empty()) {
            return "[]";
        }

        ss += "[";

        for (l_it = tlist->begin(); l_it != tlist->end(); ++l_it) {
            ss += (l_it == tlist->begin() ? "" : ",");
            ss += " " + l_it->str(nest - 1);
        }

        ss += " ]";
        return ss;

    default:
        if (base->m_type & IT) {
            return "[iterator]";
        }

        return "unknown";
    }
}

Token *PackToken::release() &&
{
    Token *b = m_base;
    // Setting base to 0 leaves the class in an invalid state,
    // except for destruction.
    m_base = nullptr;
    return b;
}
