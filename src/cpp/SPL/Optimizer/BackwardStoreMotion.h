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

#ifndef BACKWARDSTOREMOTION_H
#define BACKWARDSTOREMOTION_H

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/Runtime/Utility/Singleton.h>
#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {

//! Operator Graph Class
class DLL_PUBLIC BackwardStoreMotion : public Singleton<BackwardStoreMotion>
{

  public:
    /// Constructor
    /// Initialize state
    BackwardStoreMotion()
      : Singleton<BackwardStoreMotion>(*this)
      , _currentStmt(NULL)
      , _traceBackwardStoreMotion(false)
      , _outerLoops(false)
      , _loopDeclaration(false)
      , _foundInnerLoop(false)
    {}

    // main entry point when invoked for a SPL function
    void backwardStoreMotionInFunction(BlockStatement& blockStmt);

    // main entry point when invoked for an OnTuple clause
    void backwardStoreMotionInOnTuple(Statement*& stmt, Statement*& stateStmt, int32_t portNo);

    // type of symbol reference within a stmt
    enum ReferenceType
    {
        Use = 1,  // Symbol is used
        Def = 2,  // Symbol is defined
        Ref = 3,  // Symbol is both used and defined
        Decl = 4, // Symbol is declared
        lastReferenceType
    };

    // location, sequence and type of a single reference to a symbol
    class SingleReference
    {
      public:
        SingleReference(bool sideNode, Statement& stmt, int32_t stmtSeq, ReferenceType refType)
          : _sideNode(sideNode)
          , _stmt(&stmt)
          , _stmtSeq(stmtSeq)
          , _refType(refType)
        {}

        bool _sideNode;
        Statement* _stmt;
        int32_t _stmtSeq;
        enum ReferenceType _refType;
    };

    // vector of all references for a symbol
    typedef std::vector<SingleReference> References;

    // all references for all symbols
    class AllReferences
    {
      public:
        AllReferences(Statement& loopStmt)
          : _loopStmt(loopStmt)
        {}

        ~AllReferences(); // delete data in map

        void addReference(const std::string& refName,
                          ReferenceType refType,
                          Statement& stmt,
                          int32_t stmtSeq,
                          bool sideNode)
        {
            std::tr1::unordered_map<std::string, References*>::iterator ix =
              _allReferences.find(refName);
            if (ix == _allReferences.end()) {
                References* refPtr = new References;
                refPtr->push_back(SingleReference(sideNode, stmt, stmtSeq, refType));
                _allReferences.insert(make_pair(refName, refPtr));
            } else {
                References* refPtr = ix->second;
                refPtr->push_back(SingleReference(sideNode, stmt, stmtSeq, refType));
            }
        }
        References* findReference(const std::string& refName) const
        {
            std::tr1::unordered_map<std::string, References*>::const_iterator ix =
              _allReferences.find(refName);
            if (ix == _allReferences.end()) {
                return NULL;
            }
            return ix->second;
        }
        bool removeReference(const std::string& id, Statement& storeStmt)
        {
            References* refp = findReference(id);
            if (refp == NULL) {
                return false;
            }
            References& refs = *refp;
            for (References::iterator itr = refs.begin(); itr != refs.end();) {
                if (itr->_stmt == &storeStmt) {
                    itr = refs.erase(itr);
                } else {
                    ++itr;
                }
            }
            return true;
        }
        SingleReference* findDeclaration(const std::string& id) const
        {
            References* useRefs = findReference(id);
            if (!useRefs) {
                return NULL;
            }
            for (References::iterator itr = useRefs->begin(); itr != useRefs->end(); ++itr) {
                if (itr->_refType == Decl) {
                    return &(*itr);
                }
            }
            return NULL;
        }

        void dump(void);

      private:
        Statement& _loopStmt;
        std::tr1::unordered_map<std::string, References*> _allReferences;
    };

    // backward store motion candidate:  location and seq number of constant attr init statement
    class BsmCandidate
    {
      public:
        BsmCandidate(Statement& stmt, int32_t stmtSeq)
          : _storeStmt(&stmt)
          , _stmtSeq(stmtSeq){};

        Statement* _storeStmt;
        int32_t _stmtSeq;
    };

    // location and seq number of continue, break and return stmts in a loop
    class ImpliedCtlFlow
    {
      public:
        ImpliedCtlFlow(Statement& stmt, int32_t stmtSeq)
          : _impliedFlowStmt(&stmt)
          , _stmtSeq(stmtSeq){};

        Statement* _impliedFlowStmt;
        int32_t _stmtSeq;
    };

    // each loop has a BsmContext.
    // Loops can be nested. So child to parent and parent to child links exists.
    //
    // Backward Store Motion candidates, implied control flow stmts, side node info and all symbol
    // reference information are found in the BsmContext
    class BsmContext
    {
      public:
        BsmContext(Statement& loopStmtp, BsmContext* parentPtr)
          : _loopStmt(loopStmtp)
          , _loopStmtSeq(0)
          , _stmtSeq(0)
          , _sideNode(0)
          , _parent(parentPtr)
          , _allReferences(loopStmtp)
        {
            if (parentPtr) {
                _loopStmtSeq = parentPtr->_stmtSeq;
                parentPtr->_childLoopList.push_back(this);
                _stmtSeq = parentPtr->_stmtSeq;
            }
        }
        int32_t& stmtSeq() { return _stmtSeq; }
        int32_t& loopStmtSeq() { return _loopStmtSeq; }
        Statement& loopStmt() { return _loopStmt; }
        void pushSideNode() { ++_sideNode; }
        void popSideNode() { --_sideNode; }
        bool isSideNode() const { return _sideNode > 0; }
        void clearSideNode() { _sideNode = 0; }
        void resetSeq()
        {
            if (_parent) {
                _stmtSeq = _parent->_stmtSeq;
            } else {
                _stmtSeq = 0;
            }
        }
        BsmContext* getParent() { return _parent; }
        void addCandidate(const BsmCandidate& cand) { _candidateList.push_back(cand); }
        std::vector<BsmCandidate>& getCandidates() { return _candidateList; }
        std::vector<BsmContext*>& getChildLoops() { return _childLoopList; }
        void addImpliedCtlFlow(Statement& stmt)
        {
            _impliedCtlFlowList.push_back(ImpliedCtlFlow(stmt, _stmtSeq));
        }
        const std::vector<ImpliedCtlFlow>& getImpliedFlow() { return _impliedCtlFlowList; }
        AllReferences& getAllReferences() { return _allReferences; }

      private:
        Statement& _loopStmt;
        int32_t _loopStmtSeq;
        int32_t _stmtSeq;
        int32_t _sideNode;
        BsmContext* _parent;
        std::vector<BsmCandidate> _candidateList;
        std::vector<ImpliedCtlFlow> _impliedCtlFlowList;
        std::vector<BsmContext*> _childLoopList;
        AllReferences _allReferences;
    };

  private:
    void gatherAllReferencesInExpression(const Expression& expr,
                                         ReferenceType refType,
                                         BsmContext& bsm);
    void backwardStoreMotion(Statement*& stmt, BsmContext& bsm);
    Statement* backwardStoreMotionOutOfLoop(BsmContext& bsm);
    void gatherAllReferencesInLoop(Statement& stmt, BsmContext& bsm);
    std::string candidateStoreId(Statement& storeStmt)
    {
        ExpressionStatement& exprStmt = storeStmt.as<ExpressionStatement>();
        BinaryExpression& assignExpr = exprStmt.getExpression().as<BinaryExpression>();
        Expression& lhs = assignExpr.getLhs();
        return lhs.toString();
    }
    void removeStmtFromBlock(Statement& blockStmt, std::tr1::unordered_set<Statement*>& removeStmt);
    bool directFlowDependent(std::string& id, int32_t storeSeq, BsmContext& bsm);
    bool directFlowDependentChild(std::string& id,
                                  int32_t storeSeq,
                                  BsmContext& bsm,
                                  int32_t loopStmtSeq);
    std::string addNamePrefix(const std::string& id);

    void renameStateVariables(Literal& lit);
    void renameStateVariables(Expression& expr);
    void renameStateVariables(Statement& stmt);

    LiteralExpression& createTupleLiteralExpression(const std::string& id,
                                                    Expression& initExpr,
                                                    std::vector<BsmCandidate>& candidates);
    Literal* generateDefaultLiteral(const RootTyp& litType);
    bool supportedTupleInitType(const RootTyp& declType);
    bool supportedAttributeInitType(const RootTyp& litType);
    void adjustChildLoopStmtSeq(Statement& stmt, BsmContext& bsm);

    Statement* _currentStmt;
    Statement* _stateStmt;

    int32_t _portNo;

    std::tr1::unordered_set<std::string> _renames;

    bool _traceBackwardStoreMotion; // trace flag
    bool _outerLoops;
    bool _loopDeclaration;
    bool _foundInnerLoop;
    bool _implicitLoops;
};

};
#endif /* BACKWARDSTOREMOTION_H */
