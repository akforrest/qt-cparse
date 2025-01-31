#include "config.h"

#include "builtin-features/functions.h"
#include "builtin-features/operations.h"
#include "builtin-features/reservedwords.h"
#include "builtin-features/typespecificfunctions.h"

using namespace cparse;

Config &Config::defaultConfig()
{
    static Config conf;
    return conf;
}

Config::Config(TokenMap s, ParserMap p, OpPrecedenceMap opp, OpMap opMap)
    : scope(std::move(s)), parserMap(std::move(p)), opPrecedence(std::move(opp)), opMap(std::move(opMap))
{
}

void Config::registerBuiltInDefinitions(BuiltInDefinition def)
{
    Config &c = *this;
    builtin_functions::Register(c, def);
    builtin_operations::Register(c, def);
    builtin_reservedWords::Register(c, def);
    builtin_typeSpecificFunctions::Register(c, def);
}

void ParserMap::add(const QString &word, WordParserFunc *parser)
{
    wmap[word] = parser;
}

void ParserMap::add(QChar c, WordParserFunc *parser)
{
    cmap[c] = parser;
}

WordParserFunc *ParserMap::find(const QString &text) const
{
    if (auto it = wmap.find(text); it != wmap.end()) {
        return it->second;
    }

    return nullptr;
}

WordParserFunc *ParserMap::find(QChar c) const
{
    if (auto it = cmap.find(c); it != cmap.end()) {
        return it->second;
    }

    return nullptr;
}

TokenMap &ObjectTypeRegistry::typeMap(TokenType type)
{
    static TokenTypeMap registry;
    return registry[type];
}

Config::BuiltInDefinition cparse::operator|(Config::BuiltInDefinition l, Config::BuiltInDefinition r)
{
    return static_cast<Config::BuiltInDefinition>(static_cast<int>(l) | r);
}
