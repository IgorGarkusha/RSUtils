/*
 * Project: Remote Sensing Utilities (Extentions GDAL/OGR)
 * Author:  Igor Garkusha <rsutils.gis@gmail.com>
 *          Ukraine, Dnipro (Dnipropetrovsk)
 * 
 * Copyright (C) 2016, Igor Garkusha <rsutils.gis@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 * 
*/

#ifndef __STACK_H__
#define __STACK_H__

#include <stdio.h>

template <class T>
class CStack
{
	private:
		T * m_pStack;
		int m_stack_count;
		int m_stack_size;
	public:
		CStack()
		{
			m_stack_size = 0;
			m_stack_count = 0;
			m_pStack = NULL;
		}
		
		CStack(int maxCountStackElements)
		{
			create(maxCountStackElements);
		}
		
		CStack(const CStack<T>& stack)
		{
			m_stack_size = stack.size();
			m_pStack = new T[m_stack_size];
			int count = stack.countElementsInStack();
			for(m_stack_count=0; m_stack_count<=count; m_stack_count++)
			m_pStack[m_stack_count] = stack[m_stack_count];
		}
		
		virtual ~CStack()
		{
			this->destroy();
		}
		
		void create(int maxCountStackElements)
		{
			m_stack_size = maxCountStackElements;
			m_pStack = new T[m_stack_size];
			if(m_pStack == NULL) m_stack_size = 0;
			m_stack_count = 0;
		}
		
		void destroy()
		{
			if(m_pStack != NULL) { delete [] m_pStack; m_pStack = NULL; m_stack_count = 0; m_stack_size = 0; }
		}
		
		bool isEmpty()
		{
			return (m_stack_count == 0);
		}
		
		bool isFull()
		{
			return (m_stack_count == m_stack_size);
		}
		
		void push(T v)
		{
			if(m_pStack != NULL) if(m_stack_count < m_stack_size) m_pStack[m_stack_count++] = v;
		}
		
		T pop()
		{
			if(m_pStack != NULL) if(m_stack_count > 0) return m_pStack[--m_stack_count];
			return (T)(0);
		}
		
		int size() const
		{
			return m_stack_size;
		}
		
		int countElementsInStack() const
		{
			return m_stack_count-1;
		}
		
		T operator[](int index) const
		{
			if(m_pStack != NULL)
			{
				if((index>=0)&&(index<=(m_stack_count-1))) return m_pStack[index];
				else return (T)(0);
			}
			else return (T)(0);
		}
		
		void revers()
		{
			T * tmp = new T[m_stack_count];
			for(int i=0; i<m_stack_count; i++) tmp[m_stack_count-1-i] = m_pStack[i];
			for(int i=0; i<m_stack_count; i++) m_pStack[i] = tmp[i];
			delete [] tmp;
		}
		
		void printToConsoleAsFloat()
		{
			for(int i=0; i<m_stack_count; i++) printf("%lf\n", (float)m_pStack[i]);
		}
};

#endif
