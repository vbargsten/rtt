/***************************************************************************
  tag: Peter Soetens  Thu Jan 13 10:24:51 CET 2005  BufferLockFree.hpp 

                        BufferLockFree.hpp -  description
                           -------------------
    begin                : Thu January 13 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/
 
 
#include "os/oro_atomic.h"
#include "os/CAS.hpp"
#include <vector>

#pragma interface

namespace ORO_CoreLib
{

    using ORO_OS::CAS;

    /**
     * A Lock-free buffer implementation to read and write
     * data of type \a T in a FIFO way.
     * No memory allocation is done, but the maximum number
     * of threads which can access this object is defined by
     * MAX_THREADS.
     * @param T The value type to be stored in the Buffer.
     * Example : BufferLockFree<A> is a buffer which holds values of type A.
     */
    template< class T>
    class BufferLockFree
    {
    public:
        /** 
         * @brief The maximum number of threads.
         *
         * The number of threads which may concurrently access this buffer.
         * This is to be improved, although knowing the max number of
         * threads in a RT application is not so hard.
         */
        const static unsigned int MAX_THREADS = 8;
    private:
        struct Item {
            Item()  {
                //ATOMIC_INIT(count);
                atomic_set(&count,-1);
            }
            atomic_t count;  // refcount
            std::vector<T> data;
        };

        /**
         * Conversion of number of threads to number of buffers.
         */
        static const unsigned int BUF_NUM = MAX_THREADS+1;

        Item bufs[BUF_NUM];
        Item* volatile active;

    public:
        /**
         * Create a lock-free buffer wich can store \a bufsize elements.
         */
        BufferLockFree(unsigned int bufsize) {
            for (unsigned int i=0; i < BUF_NUM; ++i) {
                bufs[i].data.reserve( bufsize ); // pre-allocate
            }
            // bootstrap the first buffer :
            active = bufs;
            atomic_inc( &active->count );
        }

        /**
         * Write a single value to the buffer.
         * @param d the value to write
         * @return false if the buffer is full.
         */
        bool write(const T& d) 
        {
            Item* usingbuf = findEmptyBuf(); // find unused Item in bufs
            Item* orig=0;
            do {
                if (orig)
                    atomic_dec(&orig->count);
                orig = lockAndGetActive();
                if ( orig->data.size() == orig->data.capacity() ) { // check for full
                    atomic_dec( &orig->count );
                    atomic_dec( &usingbuf->count );
                    return false;
                }
                usingbuf->data = orig->data;
                usingbuf->data.push_back( d );
            } while ( CAS(&active, orig, usingbuf ) ==false);
            atomic_dec( &orig->count ); // lockAndGetActive()
            atomic_dec( &orig->count ); // set buffer free
            return true;
        }

        /**
         * Write a sequence of values to the buffer.
         * @param d the values to write
         * @return the number of values written (may be less than d.size())
         */
        int write(const std::vector<T>& d) 
        {
            Item* usingbuf = findEmptyBuf(); // find unused Item in bufs
            Item* orig=0;
            int towrite  = d.size();
            do {
                if (orig)
                    atomic_dec(&orig->count);
                orig = lockAndGetActive();
                int maxwrite = orig->data.capacity() - orig->data.size();
                if ( maxwrite == 0 ) {
                    atomic_dec( &orig->count );
                    atomic_dec( &usingbuf->count );
                    return 0;
                }
                if ( towrite > maxwrite )
                    towrite = maxwrite;
                usingbuf->data = orig->data;
                usingbuf->data.insert( usingbuf->data.end(), d.begin(), d.begin() + towrite );
            } while ( CAS(&active, orig, usingbuf ) ==false );
            atomic_dec( &orig->count ); // lockAndGetActive()
            atomic_dec( &orig->count ); // set buffer free
            return towrite;
        }

        /**
         * Read the oldest value from the buffer.
         * @param res is to be filled with a value from the buffer.
         * @return true if something was read.
         */
        bool read(T& res)
        {
            Item* nextbuf = findEmptyBuf(); // find unused Item in bufs
            nextbuf->data.clear();
            Item* orig=0;
            do {
                if (orig)
                    atomic_dec(&orig->count);
                orig = lockAndGetActive();
                if ( orig->data.empty() ) {
                    atomic_dec( &orig->count );
                    atomic_dec( &nextbuf->count );
                    return false;
                }
                res = orig->data.front();
                nextbuf->data.insert( nextbuf->data.begin(), orig->data.begin()+1, orig->data.end() );
            } while ( CAS(&active, orig, nextbuf ) ==false );
            atomic_dec( &orig->count ); // lockAndGetActive
            atomic_dec( &orig->count ); // ref count
            return true;
        }

        /**
         * Read the whole buffer.
         * @param res is to be filled with all values in the buffer,
         * with res.begin() the oldest value.
         * @return the number of items read.
         */
        int read(std::vector<T>& res)
        {
            Item* nextbuf = findEmptyBuf(); // find unused Item in bufs
            nextbuf->data.clear(); // an empty buffer will become active next.
            Item* orig=0;
            do {
                if (orig)
                    atomic_dec(&orig->count);
                orig = lockAndGetActive();
                res = orig->data;
            } while ( CAS(&active, orig, nextbuf ) == false );
            atomic_dec( &orig->count ); //lockAndGetActive
            atomic_dec( &orig->count ); //ref count
            return res.size();
        }

    private:
        // If MAX_THREADS is large enough, this will always succeed :
        Item* findEmptyBuf() {
            Item* start = bufs;
            while( true ) {
                if ( atomic_inc_and_test( &start->count ) )
                    break;
                atomic_dec( &start->count );
                ++start;
                if (start == bufs+BUF_NUM)
                    start = bufs; // in case of races, rewind
            }
            return start; // unique pointer across all threads
        }

        // This is a kind-of smart-pointer implementation
        // We could move it into Item itself and overload operator=
        Item* lockAndGetActive() {
            Item* orig=0;
            do {
                if (orig)
                    atomic_dec( &orig->count );
                orig = active;
                atomic_inc( &orig->count );
            } while ( CAS(&active, orig, orig ) == false );
            return orig;
        }

    };

    template<class T>
    const unsigned int BufferLockFree<T>::MAX_THREADS;

    template<class T>
    const unsigned int BufferLockFree<T>::BUF_NUM;

    extern template class BufferLockFree<double>;
}
