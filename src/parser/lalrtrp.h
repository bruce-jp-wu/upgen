/*
    Upgen -- a scanner and parser generator.
    Copyright (C) 2009-2018 Bruce Wu
    
    This file is a part of Upgen program

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef PARSER_LALRTRIPLE_H__
#define PARSER_LALRTRIPLE_H__

#include "../common/types.h"
#include <functional>
using std::binary_function;
#include <algorithm>
#include <limits.h>
#include <assert.h>
#include <ostream>
#include <iomanip>

namespace parser_ns{

/////////////////////////////////////////////////////////////////////
// CRC enconder
class CRCCoder {
public:
    CRCCoder()
        :mCRCCode(0xffffffff){
        init();
    }

    void beginCode(void) {
        mCRCCode = 0xffffffff;
    }
    void update(int nVal) {
        unsigned char *p = (unsigned char *)&nVal;
        update(p, (int)sizeof(int));
    }
    void update(const unsigned char *buf, const int size) {
        for(int i = 0; i < size; i++) {
           mCRCCode = (mCRCCode >> 8) ^ mCRCTab[(mCRCCode & 0xFF) ^ buf[i]];
        }
    }
    unsigned int getCode(void) const {
        return mCRCCode ^ 0xffffffff;
    }

private:
    unsigned int calcCode(const unsigned char *buf, const int size) {
        unsigned int crcCode  = 0xffffffff;
        for(int i = 0; i < size; i++) {
           crcCode = (crcCode >> 8) ^ mCRCTab[(crcCode & 0xFF) ^ buf[i]];
        }
        return crcCode ^ 0xffffffff;
    }

    void init(void) {
        for (unsigned int i = 0; i < 256; i++) {
            unsigned int crcCode = i;
            for (unsigned int j = 0; j < 8; j++) {
                if (crcCode & 1) {
                    crcCode = (crcCode >> 1) ^ 0xEDB88320;
                } else {
                    crcCode >>= 1;
                }
            }
            mCRCTab[i] = crcCode;
        }
    }

private:
    unsigned int mCRCTab[256];
    unsigned int mCRCCode;
};




struct LR0Item {
    LR0Item(int nRule = -1, int nDot = -1)
        : mRuleID(nRule)
        , mDotPos(nDot) {

    }
    int mRuleID;
    int mDotPos;
};

class LR0ItemLess: public binary_function<LR0Item, LR0Item, bool> {
public:
    bool operator() (const LR0Item &a, const LR0Item &b) const {
        return a.mRuleID < b.mRuleID
                || (a.mRuleID == b.mRuleID && a.mDotPos < b.mDotPos);
    }
};

class LR0ItemEqual: public binary_function<LR0Item, LR0Item, bool> {
public:
    bool operator() (const LR0Item &a, const LR0Item &b) const {
        return a.mRuleID == b.mRuleID && a.mDotPos == b.mDotPos;
    }
};


typedef std::map<LR0Item, sint_t, LR0ItemLess> LALRItemMap;

///////////////////////////////////////////////////////////////////////
class KernelItemsLess;

class KernelItems {
    friend class KernelItemsLess;
public:
    typedef LALRItemMap item_map_t;
    typedef item_map_t::iterator item_it_t;
    typedef item_map_t::const_iterator item_cit_t;
public:
    KernelItems()
        : mLR0Code(0)
        , mLR1Code(0) {

    }

    item_cit_t beginItems(void) const {
        return mItems.cbegin();
    }
    item_it_t beginItems(void) {
        return mItems.begin();
    }
    item_cit_t endItems(void) const {
        return mItems.cend();
    }
    item_it_t endItems(void) {
        return mItems.end();
    }


    std::pair<item_it_t, bool> insert(const LR0Item& lr0Item, const sint_t &laSet) {
        return mItems.insert(std::make_pair(lr0Item, laSet));
    }

    void calcLR0Code(CRCCoder &coder) {
        coder.beginCode();
        for(item_cit_t cit = mItems.cbegin();
            cit != mItems.end();
            cit++) {

            coder.update(cit->first.mRuleID);
            coder.update(cit->first.mDotPos);
        }

        mLR0Code = coder.getCode();
    }

    void calcLR1Code(CRCCoder &coder) {
        coder.beginCode();
        for(item_cit_t cit = mItems.cbegin();
            cit != mItems.end();
            cit++) {

            coder.update(cit->first.mRuleID);
            coder.update(cit->first.mDotPos);

            for(sint_cit_t lacit = cit->second.begin();
                lacit != cit->second.end();
                lacit++) {
                coder.update(*lacit);
            }
        }

        mLR1Code = coder.getCode();
    }

private:
    item_map_t mItems;
    unsigned int mLR0Code;
    unsigned int mLR1Code;
};

class KernelItemsLess: public binary_function<KernelItems, KernelItems, bool> {
public:
    bool operator()(const KernelItems &aKI, const KernelItems &bKI) const {
        if(aKI.mLR0Code != bKI.mLR0Code) {
            return aKI.mLR0Code < bKI.mLR0Code;
        } if(aKI.mLR1Code != bKI.mLR1Code) {
            return aKI.mLR1Code < bKI.mLR1Code;
        } else {
            LR0ItemEqual equalComparer;
            LR0ItemLess lessComparer;
            KernelItems::item_cit_t cita = aKI.mItems.cbegin();
            KernelItems::item_cit_t citb = bKI.mItems.cbegin();
            for(; cita != aKI.mItems.end() && citb != bKI.mItems.end();
                cita++, citb++) {
                if(! equalComparer(cita->first, citb->first)) {
                    return lessComparer(cita->first, citb->first);
                }
            }
            return cita == aKI.mItems.end() && citb != bKI.mItems.end();
        }
    }
};




struct LALRGraphArc {

    LALRGraphArc(int nSymb, int nToState)
        : mSymbol(nSymb)
        , mToState(nToState) {

    }
    int mSymbol;
    int mToState;
};

class LGARcLess: public binary_function<LALRGraphArc, LALRGraphArc, bool> {
public:
    bool operator()(const LALRGraphArc &a, const LALRGraphArc &b) const {
        return a.mSymbol < b.mSymbol
                || (a.mSymbol == b.mSymbol && a.mToState < b.mToState);
    }
};



class LR0NodeLess;
class LR0NodeEqual;
//class LALRNodeLess;

class LALRGraphNode {
public:
    typedef LALRItemMap item_map_t;
    typedef item_map_t::iterator item_it_t;
    typedef item_map_t::const_iterator item_cit_t;


    typedef std::vector<LALRGraphArc> arc_set_t;
    typedef arc_set_t::iterator arc_it_t;
    typedef arc_set_t::const_iterator arc_cit_t;
public:
    LALRGraphNode(int nID)
        : mLR0Code(0)
        , mLR1Code(0)
        , mID(nID) {

    }

    std::pair<item_it_t, bool> insertItem(int nRuleID, int nDotPos) {
        LR0Item item;
        item.mRuleID = nRuleID;
        item.mDotPos = nDotPos;
        return mItems.insert(std::make_pair(item, sint_t()));
    }

    void addLRItems(const KernelItems &kitems) {
        mItems.insert(kitems.beginItems(), kitems.endItems());
    }

    LALRGraphArc& addArc(int nSymb, int nToState) {
        mArcs.push_back(LALRGraphArc(nSymb, nToState));
        return mArcs.back();
    }


    int getID(void) const {
        return mID;
    }
    void setID(int nID) {
        mID = nID;
    }

    item_cit_t beginItems(void) const {
        return mItems.cbegin();
    }
    item_it_t beginItems(void) {
        return mItems.begin();
    }
    item_cit_t endItems(void) const {
        return mItems.cend();
    }
    item_it_t endItems(void) {
        return mItems.end();
    }


    arc_cit_t beginArcs(void) const {
        return mArcs.cbegin();
    }
    arc_it_t beginArcs(void) {
        return mArcs.begin();
    }
    arc_cit_t endArcs(void) const {
        return mArcs.cend();
    }
    arc_it_t endArcs(void) {
        return mArcs.end();
    }


    void calcLR0Code(CRCCoder &coder) {
        coder.beginCode();
        for(item_cit_t cit = mItems.cbegin();
            cit != mItems.cend();
            cit++) {

            coder.update(cit->first.mRuleID);
            coder.update(cit->first.mDotPos);
        }

        mLR0Code = coder.getCode();
    }

    void calcLR1Code(CRCCoder &coder) {
        coder.beginCode();
        for(item_cit_t cit = mItems.cbegin();
            cit != mItems.cend();
            cit++) {

            coder.update(cit->first.mRuleID);
            coder.update(cit->first.mDotPos);

            for(sint_cit_t lacit = cit->second.begin();
                lacit != cit->second.end();
                lacit++) {
                coder.update(*lacit);
            }
        }

        mLR1Code = coder.getCode();
    }

    void merge(LALRGraphNode &aNode) {
        assert(mItems.size() == aNode.mItems.size());
        for(item_it_t ita = aNode.mItems.begin();
            ita != aNode.mItems.end();
            ita++) {

            item_it_t itThis = mItems.find(ita->first);
            assert(itThis != mItems.end());

            sint_t& lookheads = itThis->second;
            lookheads.insert(ita->second.cbegin(),
                                       ita->second.cend());
        }
    }

    void relabel(i2i_map_t &relabelMap) {
        std::set<LALRGraphArc, LGARcLess> updateArcs;
        int sz = (int)mArcs.size();
        for(int i = 0; i < sz; i++) {
            LALRGraphArc arc(
                        mArcs[i].mSymbol,
                        relabelMap[mArcs[i].mToState]);
            updateArcs.insert(arc);
        }

        mArcs.clear();
        std::copy(updateArcs.begin(), updateArcs.end(), std::back_inserter(mArcs));
    }

    friend std::ostream& operator<<(std::ostream& os, LALRGraphNode &node) {
        os << "Node. " << node.mID << std::endl;
        os << "Items" << std::endl;
        for(LALRGraphNode::item_cit_t cit = node.mItems.begin();
            cit != node.mItems.end();
            cit++) {
            os << "\t" << cit->first.mRuleID << ":" << cit->first.mDotPos << "\t[";
            for(sint_cit_t laCit = cit->second.begin();
                laCit != cit->second.end();
                laCit++) {
                os << *laCit << " ";
            }
            os << "]" << std::endl;
        }
        os << "Transitions" << std::endl;
        for(int i = 0; i < (int)node.mArcs.size(); i++) {
            os << "Symbol: " << node.mArcs[i].mSymbol << " ==> State "
               << node.mArcs[i].mToState << std::endl;
        }
        os << std::endl;
        return os;
    }

private:
    friend class LR0NodeLess;
    friend class LR0NodeEqual;
//    friend class LALRNodeLess;

private:
    arc_set_t mArcs;
    item_map_t mItems;
    unsigned int mLR0Code;
    unsigned int mLR1Code;
    int mID;
};

class LR0NodeLess: public binary_function<LALRGraphNode*, LALRGraphNode*, bool> {
public:
    bool operator()(const LALRGraphNode* aNode, const LALRGraphNode* bNode) const {
        if(aNode->mLR0Code != bNode->mLR0Code) {
            return aNode->mLR0Code < bNode->mLR0Code;
        } else {
            LR0ItemEqual equalComparer;
            LR0ItemLess lessComparer;
            LALRGraphNode::item_cit_t cita = aNode->mItems.begin();
            LALRGraphNode::item_cit_t citb = bNode->mItems.begin();
            for(; cita != aNode->mItems.end() && citb != bNode->mItems.end();
                cita++, citb++) {
                if(! equalComparer(cita->first, citb->first)) {
                    return lessComparer(cita->first, citb->first);
                }
            }
            return cita == aNode->mItems.end() && citb != bNode->mItems.end();
        }
    }
};
class LR0NodeEqual: public binary_function<LALRGraphNode*, LALRGraphNode*, bool> {
public:
    bool operator()(const LALRGraphNode* aNode, const LALRGraphNode* bNode) const {
        if(aNode->mLR0Code != bNode->mLR0Code) {
            return false;
        } else {
            LR0ItemEqual equalComparer;
            LALRGraphNode::item_cit_t cita = aNode->mItems.begin();
            LALRGraphNode::item_cit_t citb = bNode->mItems.begin();
            for(; cita != aNode->mItems.end() && citb != bNode->mItems.end();
                cita++, citb++) {
                if(! equalComparer(cita->first, citb->first)) {
                    return false;
                }
            }
            return cita == aNode->mItems.end() && citb == bNode->mItems.end();
        }
    }
};


class LALRGraph {
public:
    LALRGraph()
        : mCRCCoder() {

    }

    LALRGraphNode& addNode(void) {
        int nID = (int)mNodes.size();
        mNodes.push_back(new LALRGraphNode(nID));
        return *mNodes.back();
    }

    LALRGraphNode& addNode(const KernelItems &kitems) {
        int nID = (int)mNodes.size();
        mNodes.push_back(new LALRGraphNode(nID));
        mNodes.back()->addLRItems(kitems);
        return *mNodes.back();
    }

    LALRGraphNode& getNode(int nID) {
        assert(nID >= 0 && nID < (int)mNodes.size());
        return *mNodes[nID];
    }

    int getNodeCount(void) const {
        return (int)mNodes.size();
    }


    CRCCoder& getCRCCoder(void) {
        return mCRCCoder;
    }

    void kernelMerge(void) {
        if(mNodes.size() < 3) {
            return;
        }

        // sort (group) by LR(0) items
        LR0NodeLess lessCmp;
        std::sort(mNodes.begin(), mNodes.end(), lessCmp);

        i2i_map_t relabelMap;

        LR0NodeEqual equalCmp;
        int minID = mNodes[0]->getID(), maxID = mNodes[0]->getID();
        const int sz = (int)mNodes.size();
        for(int begIdx = 0, endIdx = 1, smallestIdx = begIdx; ; endIdx++) {

            if(endIdx >= sz || !equalCmp(mNodes[begIdx], mNodes[endIdx])) {
                const int leaderID = mNodes[smallestIdx]->getID();
                std::swap(mNodes[begIdx], mNodes[smallestIdx]);

                relabelMap[leaderID] = leaderID;
                for(int mIdx = begIdx + 1; mIdx < endIdx; mIdx++) {
                    relabelMap[mNodes[mIdx]->getID()] = leaderID;
                    mNodes[begIdx]->merge(*mNodes[mIdx]);
                    delete mNodes[mIdx];
                    mNodes[mIdx] = nullptr;
                }

                begIdx = endIdx;
                smallestIdx = begIdx;
            } else {
                if(mNodes[smallestIdx]->getID() > mNodes[endIdx]->getID()) {
                    smallestIdx = endIdx;
                }
            }

            if(endIdx >= sz) {
                break;
            } else {
                if(minID > mNodes[endIdx]->getID()) {
                    minID = mNodes[endIdx]->getID();
                }
                if(maxID < mNodes[endIdx]->getID()) {
                    maxID = mNodes[endIdx]->getID();
                }
            }
        }


        int newSize = 1;
        for(int i = 1; i < sz; i++) {
            if(mNodes[i] != nullptr) {
                mNodes[newSize++] = mNodes[i];
            }
        }

        mNodes.resize(newSize);
        std::sort(mNodes.begin(), mNodes.end(),
                  [](const LALRGraphNode *aNode, const LALRGraphNode *bNode) {
            return aNode->getID() < bNode->getID();
        });


        // update relabel map
        i2i_map_t leaderMap;
        for(int i = 0; i < newSize; i++) {
            leaderMap[mNodes[i]->getID()] = i;
            mNodes[i]->setID(i);
        }

        for(int i = minID; i <= maxID; i++) {
            if(relabelMap.find(i) != relabelMap.end()) {
                int oldLeaderID = relabelMap[i];
                relabelMap[i] = leaderMap[oldLeaderID];
            }
        }


        for(int i = 0; i < newSize; i++) {
            mNodes[i]->relabel(relabelMap);
        }

    }


    friend std::ostream& operator<<(std::ostream& os, LALRGraph &g) {
        os << "Node Count: " << g.mNodes.size() << std::endl;
        for(int i = 0; i < (int)g.mNodes.size(); i++) {
            if(g.mNodes[i]) {
                os << std::endl << *g.mNodes[i] << std::endl;
            }
        }
        return os;
    }
private:
    std::vector<LALRGraphNode*> mNodes;
    CRCCoder mCRCCoder;
};


// update 18/06/10
#if 0
// triple <left-symbol, first-right-symbol, rule-id>
class lalrtrp_t {
	
private:
	
	mutable sint_t *m_psi;
	sint_t m_siEmpty;
	i2si_map_t m_left2Firsts;
	i2si_map_t m_first2Rules;
	
public:
	
	typedef sint_it_t iterator;
	typedef sint_cit_t const_iterator;
public:
	
	inline lalrtrp_t(void) {
		m_psi = &m_siEmpty;
	}
	inline ~lalrtrp_t(void) {
		clear();
	}
	void clear(void) {
		
		m_psi = &m_siEmpty;
		i2si_it_t it;
		for(it = m_left2Firsts.begin(); it != m_left2Firsts.end(); ++it) {
			delete it->second;
		}
		m_left2Firsts.clear();
		for(it = m_first2Rules.begin(); it != m_first2Rules.end(); ++it) {
			delete it->second;
		}
		m_first2Rules.clear();
	}
	inline int size() const {
		return (int)m_left2Firsts.size();
	}
	
	inline bool addTriple(int a_nLeft, int a_nFirst, int a_nRule) {
		
		bool bRet = false;
		pair<i2si_it_t, bool> pairRet = m_left2Firsts.insert(i2si_pair_t(a_nLeft, nullptr));
		
		if( ! pairRet.second) {
			bRet = pairRet.first->second->insert(a_nFirst).second;
		}
		else {
			bRet = true;
			pairRet.first->second = new sint_t;
			pairRet.first->second->insert(a_nFirst);			
		}
		
		pair<i2si_it_t, bool> pairRet2 = m_first2Rules.insert(i2si_pair_t(a_nFirst, nullptr));
		if( ! pairRet2.second) {
			bRet = pairRet2.first->second->insert(a_nRule).second || bRet;
		}
		else {
			bRet = true;
			pairRet2.first->second = new sint_t;
			pairRet2.first->second->insert(a_nRule);
		}

		return bRet;
	}
	
	inline bool addTriples(int a_nDestLeft, int a_nSrcLeft) {
		
		i2si_cit_t citSrc = m_left2Firsts.find(a_nSrcLeft);
		if(citSrc != m_left2Firsts.end()) {
			
			i2si_it_t itDest = m_left2Firsts.find(a_nDestLeft);
			if(itDest != m_left2Firsts.end()) {
				
				unsigned int nOldCnt = itDest->second->size();
				itDest->second->insert(citSrc->second->begin(), citSrc->second->end());
				return itDest->second->size() > nOldCnt;
			}
		}
		
		return false;
	}
	
	inline iterator firstBegin(int a_nLeft) {
		
		i2si_it_t it = m_left2Firsts.find(a_nLeft);
		if(it != m_left2Firsts.end()) {
			m_psi = it->second;
		}
		else {
			m_psi = &m_siEmpty;
		}
		return m_psi->begin();
	}
	inline const_iterator firstBegin(int a_nLeft) const {

		i2si_cit_t cit = m_left2Firsts.find(a_nLeft);
		if(cit != m_left2Firsts.end()) {
			m_psi = cit->second;
		}
		else {
			m_psi = const_cast<sint_t*>(&m_siEmpty);
		}
		return m_psi->begin();
	}
	
	inline iterator firstEnd(int a_nLeft) {
		
		return m_psi->end();
	}
	inline const_iterator firstEnd(int a_nLeft) const {
		
		return m_psi->end();
	}
	
	inline sint_t* getRules(int a_nLeft, int a_nFirst) {
		
		if(m_left2Firsts.find(a_nLeft) != m_left2Firsts.end()) {
			
			i2si_it_t it = m_first2Rules.find(a_nFirst);
			if(it != m_first2Rules.end()) {
				return it->second;
			}
		}
		return nullptr;
	}
	inline const sint_t* getRules(int a_nLeft, int a_nFirst) const {
		
		if(m_left2Firsts.find(a_nLeft) != m_left2Firsts.end()) {
			
			i2si_cit_t cit = m_first2Rules.find(a_nFirst);
			if(cit != m_first2Rules.end()) {
				return cit->second;
			}
		}
		return nullptr;
	}
};
#endif

}

#endif // PARSER_LALRTRIPLE_H__
