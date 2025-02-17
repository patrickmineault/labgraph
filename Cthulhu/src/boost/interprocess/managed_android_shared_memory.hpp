// Copyright 2004-present Facebook. All Rights Reserved.

#ifndef BOOST_INTERPROCESS_MANAGED_ANDROID_SHARED_MEMORY_HPP
#define BOOST_INTERPROCESS_MANAGED_ANDROID_SHARED_MEMORY_HPP

#ifndef BOOST_CONFIG_HPP
#include <boost/config.hpp>
#endif

#if defined(BOOST_HAS_PRAGMA_ONCE)
#pragma once
#endif

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>

#include <boost/interprocess/creation_tags.hpp>
#include <boost/interprocess/detail/managed_memory_impl.hpp>
#include "android_shared_memory.hpp"
#include "detail/managed_open_or_create_impl_ashmem.hpp"
// These includes needed to fulfill default template parameters of
// predeclarations in interprocess_fwd.hpp
#include <boost/interprocess/mem_algo/rbtree_best_fit.hpp>
#include <boost/interprocess/sync/mutex_family.hpp>

namespace boost {
namespace interprocess {

namespace ipcdetail {

template <class AllocationAlgorithm>
struct android_shmem_open_or_create {
  typedef ipcdetail::managed_open_or_create_impl_ashmem<
      android_shared_memory,
      AllocationAlgorithm::Alignment,
      true,
      false>
      type;
};

} // namespace ipcdetail

//! A basic shared memory named object creation class. Initializes the
//! shared memory segment. Inherits all basic functionality from
//! basic_managed_memory_impl<CharType, AllocationAlgorithm, IndexType>*/
template <class CharType, class AllocationAlgorithm, template <class IndexConfig> class IndexType>
class basic_android_managed_shared_memory
    : public ipcdetail::basic_managed_memory_impl<
          CharType,
          AllocationAlgorithm,
          IndexType,
          ipcdetail::android_shmem_open_or_create<
              AllocationAlgorithm>::type::ManagedOpenOrCreateUserOffset>,
      private ipcdetail::android_shmem_open_or_create<AllocationAlgorithm>::type {
#if !defined(BOOST_INTERPROCESS_DOXYGEN_INVOKED)
  typedef ipcdetail::basic_managed_memory_impl<
      CharType,
      AllocationAlgorithm,
      IndexType,
      ipcdetail::android_shmem_open_or_create<
          AllocationAlgorithm>::type::ManagedOpenOrCreateUserOffset>
      base_t;
  typedef typename ipcdetail::android_shmem_open_or_create<AllocationAlgorithm>::type base2_t;

  typedef ipcdetail::create_open_func<base_t> create_open_func_t;

  basic_android_managed_shared_memory* get_this_pointer() {
    return this;
  }

 public:
  typedef android_shared_memory device_type;
  typedef typename base_t::size_type size_type;

 private:
  typedef typename base_t::char_ptr_holder_t char_ptr_holder_t;
  BOOST_MOVABLE_BUT_NOT_COPYABLE(basic_android_managed_shared_memory)
#endif //#ifndef BOOST_INTERPROCESS_DOXYGEN_INVOKED

 public: // functions
  //! Destroys *this and indicates that the calling process is finished using
  //! the resource. The destructor function will deallocate
  //! any system resources allocated by the system for use by this process for
  //! this resource. The resource can still be opened again calling
  //! the open constructor overload. To erase the resource from the system
  //! use remove().
  ~basic_android_managed_shared_memory() {}

  //! Default constructor. Does nothing.
  //! Useful in combination with move semantics
  basic_android_managed_shared_memory() {}

  //! Creates shared memory and creates and places the segment manager.
  //! This can throw.
  basic_android_managed_shared_memory(
      create_only_t,
      const char* name,
      size_type size,
      const void* addr = 0)
      : base_t(),
        base2_t(
            create_only,
            name,
            size,
            read_write,
            addr,
            create_open_func_t(get_this_pointer(), ipcdetail::DoCreate)) {}

  //! Creates shared memory and creates and places the segment manager if
  //! segment was not created. If segment was created it connects to the
  //! segment.
  //! This can throw.
  basic_android_managed_shared_memory(
      open_or_create_t,
      const char* name,
      size_type size,
      const void* addr = 0)
      : base_t(),
        base2_t(
            open_or_create,
            name,
            size,
            read_write,
            addr,
            create_open_func_t(get_this_pointer(), ipcdetail::DoOpenOrCreate)) {}

  //! Connects to a created shared memory and its segment manager.
  //! in copy_on_write mode.
  //! This can throw.
  basic_android_managed_shared_memory(open_copy_on_write_t, const char* name, const void* addr = 0)
      : base_t(),
        base2_t(
            open_only,
            name,
            copy_on_write,
            addr,
            create_open_func_t(get_this_pointer(), ipcdetail::DoOpen)) {}

  //! Connects to a created shared memory and its segment manager.
  //! in read-only mode.
  //! This can throw.
  basic_android_managed_shared_memory(open_read_only_t, const char* name, const void* addr = 0)
      : base_t(),
        base2_t(
            open_only,
            name,
            read_only,
            addr,
            create_open_func_t(get_this_pointer(), ipcdetail::DoOpen)) {}

  //! Connects to a created shared memory and its segment manager.
  //! This can throw.
  basic_android_managed_shared_memory(open_only_t, const char* name, const void* addr = 0)
      : base_t(),
        base2_t(
            open_only,
            name,
            read_write,
            addr,
            create_open_func_t(get_this_pointer(), ipcdetail::DoOpen)) {}

  //! Moves the ownership of "moved"'s managed memory to *this.
  //! Does not throw
  basic_android_managed_shared_memory(BOOST_RV_REF(basic_android_managed_shared_memory) moved) {
    basic_android_managed_shared_memory tmp;
    this->swap(moved);
    tmp.swap(moved);
  }

  //! Moves the ownership of "moved"'s managed memory to *this.
  //! Does not throw
  basic_android_managed_shared_memory& operator=(BOOST_RV_REF(basic_android_managed_shared_memory)
                                                     moved) {
    basic_android_managed_shared_memory tmp(boost::move(moved));
    this->swap(tmp);
    return *this;
  }

  //! Swaps the ownership of the managed shared memories managed by *this and other.
  //! Never throws.
  void swap(basic_android_managed_shared_memory& other) {
    base_t::swap(other);
    base2_t::swap(other);
  }

  //! Tries to resize the managed shared memory object so that we have
  //! room for more objects.
  //!
  //! This function is not synchronized so no other thread or process should
  //! be reading or writing the file
  static bool grow(const char* shmname, size_type extra_bytes) {
    return base_t::template grow<basic_android_managed_shared_memory>(shmname, extra_bytes);
  }

  //! Tries to resize the managed shared memory to minimized the size of the file.
  //!
  //! This function is not synchronized so no other thread or process should
  //! be reading or writing the file
  static bool shrink_to_fit(const char* shmname) {
    return base_t::template shrink_to_fit<basic_android_managed_shared_memory>(shmname);
  }
#if !defined(BOOST_INTERPROCESS_DOXYGEN_INVOKED)

  //! Tries to find a previous named allocation address. Returns a memory
  //! buffer and the object count. If not found returned pointer is 0.
  //! Never throws.
  template <class T>
  std::pair<T*, size_type> find(char_ptr_holder_t name) {
    if (base2_t::get_mapped_region().get_mode() == read_only) {
      return base_t::template find_no_lock<T>(name);
    } else {
      return base_t::template find<T>(name);
    }
  }

#endif //#ifndef BOOST_INTERPROCESS_DOXYGEN_INVOKED
};

//! Typedef for a default basic_android_managed_shared_memory
//! of narrow characters
typedef basic_android_managed_shared_memory<char, rbtree_best_fit<mutex_family>, iset_index>
    managed_android_shared_memory;

//! Typedef for a default basic_android_managed_shared_memory
//! of wide characters
typedef basic_android_managed_shared_memory<wchar_t, rbtree_best_fit<mutex_family>, iset_index>
    wmanaged_android_shared_memory;

//! Typedef for a default basic_android_managed_shared_memory
//! of narrow characters to be placed in a fixed address
typedef basic_android_managed_shared_memory<char, rbtree_best_fit<mutex_family, void*>, iset_index>
    fixed_managed_android_shared_memory;

//! Typedef for a default basic_android_managed_shared_memory
//! of narrow characters to be placed in a fixed address
typedef basic_android_managed_shared_memory<
    wchar_t,
    rbtree_best_fit<mutex_family, void*>,
    iset_index>
    wfixed_managed_android_shared_memory;

} // namespace interprocess
} // namespace boost

#include <boost/interprocess/detail/config_end.hpp>

#endif // BOOST_INTERPROCESS_MANAGED_ANDROID_SHARED_MEMORY_HPP
