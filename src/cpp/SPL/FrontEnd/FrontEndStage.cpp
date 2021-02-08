/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <SPL/FrontEnd/FrontEndStage.h>

#include <cassert>

namespace SPL {
std::string FrontEndStage::toString() const
{
    switch (_value) {
        case ERROR:
            return "(errorFrontEndStage)";
        case PARSER:
            return "parser";
        case DIAGNOSTICIAN:
            return "diagnostician";
        case NAME_DEF_FINDER:
            return "nameDefFinder";
        case LEXICAL_NAME_USE_FINDER:
            return "lexicalNameUseFinder";
        case COMPOSITE_FORMAL_EXPANDER:
            return "compositeFormalExpander";
        case SCOPE_EXPANDER:
            return "scopeExpander";
        case RELATIVE_NAME_USE_FINDER:
            return "relativeNameUseFinder";
        case TYPE_ANALYZER:
            return "typeAnalyzer";
        case TYPE_PROMOTER:
            return "typePromoter";
        case PROMOTION_CHECKER:
            return "promotionChecker";
        case EXPRESSION_TYPE_FINDER:
            return "expresionTypeFinder";
        case EMPTY_CONTAINER_TYPE_FINDER:
            return "emptyContainerTypeFinder";
        case VARIABLE_USE_CHECKER:
            return "variableUseChecker";
        case LOGIC_SUBMIT_CHECKER:
            return "logicSubmitChecker";
        case OUTPUT_SIDE_EFFECTS_CHECKER:
            return "outputSideEffectsChecker";
        case DONE:
            return "(doneFrontEndStage)";
        default:
            assert(!"cannot happen");
    }
    return "";
}
}
