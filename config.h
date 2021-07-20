#ifndef CPARSE_CONFIG_H
#define CPARSE_CONFIG_H

#include <map>
#include <set>
#include <vector>

#include <QString>

#include "operation.h"

namespace cparse
{
    class RpnBuilder;

    // WordParserFunc is the function type called when
    // a reserved word or character is found at parsing time

    using WordParserFunc = bool (const char *, const char **, RpnBuilder *);
    using WordParserFuncMap = std::map<QString, WordParserFunc *>;
    using CharParserFuncMap = std::map<char, WordParserFunc *>;

    class ParserMap
    {
        public:

            // Add reserved word:
            void add(const QString & word, WordParserFunc * parser);

            // Add reserved character:
            void add(char c,  WordParserFunc * parser);

            WordParserFunc * find(const QString & text) const;
            WordParserFunc * find(char c) const;

        private:

            WordParserFuncMap wmap;
            CharParserFuncMap cmap;
    };

    using TokenTypeMap = std::map<TokenType, TokenMap>;

    class Config
    {
        public:

            enum BuiltInDefinition
            {
                NumberOperators,
                LogicalOperators,
                ContainerOperators,
                MathFunctions,
                SystemFunctions,
                AllDefinitions = NumberOperators |
                                 LogicalOperators |
                                 ContainerOperators |
                                 MathFunctions |
                                 SystemFunctions
            };

            Config() = default;
            Config(ParserMap p, OpPrecedenceMap opp, OpMap opMap);

            void registerBuiltInDefinitions(BuiltInDefinition def);

            static Config & defaultConfig();

            ParserMap parserMap;
            OpPrecedenceMap opPrecedence;
            OpMap opMap;
    };

    class ObjectTypeRegistry
    {
        public:

            static TokenMap & typeMap(TokenType type);

        private:

            ObjectTypeRegistry() = default;
    };
}
#endif // CPARSE_CONFIG_H
