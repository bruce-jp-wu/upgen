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

#ifndef COMMON_UNIQUEUE_H__
#define COMMON_UNIQUEUE_H__

#include "./types.h"
#include <set>
#include <iterator>
#include <functional>
using std::set;
using std::iterator;

namespace common_ns {

///////////////////////////////////////////////////////////////////
//class uniqueue_t definition
// uniqueue_t: queue without duplicated elements

template<class T>
class uniqueue_t {
	
private:		

	struct _node {
		
	public:
		_node(const T& e, _node* n = nullptr)
		: elem(e)
		, next(n) {
		}
		
	public:
		T elem;
		_node* next;
	};
	
	typedef set<T> tset_t;

private:
	
	// tail pointer of queue
	_node* _tail;
	// head pointer of queue
	_node* _head;
	
	// set that keeps track of element in queue
	tset_t logt;

public:
	
	inline uniqueue_t(void)
	: _tail(nullptr)
	, _head(nullptr) {
	}
	~uniqueue_t(void) {
		
		_node *p = _head, *q = nullptr;
		while(p) {
			
			q = p->next;
			delete p;
			p = q;
		}
	}
	void clear(void) {
		
		_node *p = _head, *q = nullptr;
		while(p) {
			
			q = p->next;
			delete p;
			p = q;
		}
		
		_head = _tail = nullptr;
    logt.clear();
	}
	
	inline bool isEmpty(void) const {
		return ( ! _head);
	}

	inline bool push(const T& e) {
		
		if( ! _tail) {
			_head = _tail = new _node(e);
		}
		else {
			// check if the element exists in queue
            if(logt.end() == logt.find(e)) {
            	
			    _tail->next = new _node(e);
			    _tail = _tail->next;
            }
		}
    return logt.insert(e).second;
	}
	inline bool pop(T& e) {
		
		if( ! _head) {
			return false;
		}
		
		_node* h = _head;
		if(_head == _tail) {
			_head = _tail = nullptr;
		}
		else {
			_head = _head->next;
		}

		e = h->elem;

    logt.erase(e);
		delete h;
		
		return true;
	}
	inline bool getHead(T& e) const {
		
		if( _head) {
			e = _head->elem;
		}
		return _head;
	}
	inline bool getTail(T& e) const {
		
		if( _tail ) {	
			e = _tail->elem;
		}
		return _tail;
	}
};
}

#endif // COMMON_UNIQUEUE_H__
