// -*- mode:c++ -*-
//
// Header file container_traits.hpp
//
// Traits information about entire containers for use in determining
// what Python methods to support for a container.
//
// Copyright (c) 2003 Raoul M. Gough
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy
// at http://www.boost.org/LICENSE_1_0.txt)
//
// History
// =======
// 2003/ 8/23   rmg     File creation as container_suite.hpp
// 2003/ 9/ 8   rmg     Renamed container_traits.hpp
//
// $Id$
//

#ifndef container_traits_rmg_20030823_included
#define container_traits_rmg_20030823_included

#include <boost/python/suite/indexing/suite_utils.hpp>
#include <boost/python/suite/indexing/iterator_traits.hpp>
#include <boost/python/suite/indexing/value_traits.hpp>

#include <boost/type_traits.hpp>
#include <boost/call_traits.hpp>

namespace boost { namespace python { namespace indexing {
  /////////////////////////////////////////////////////////////////////////
  // Lowest common denominator traits - applicable to real containers
  // and iterator pairs
  /////////////////////////////////////////////////////////////////////////

  template<typename Container>
  struct base_container_traits
    : public iterator_detail::traits_by_category<typename Container::iterator>
  ::type
  {
  protected:
    typedef typename
      iterator_detail::traits_by_category<typename Container::iterator>
      ::type base_type;

    BOOST_STATIC_CONSTANT (bool,   is_mutable
                           = ! boost::is_const<Container>::value);

  public:
    typedef Container container;

    typedef typename container::size_type  size_type;
    typedef typename base_type::value_type value_type; // insert, etc.
    typedef typename base_type::value_type key_type;   // find, count, ...

    typedef typename make_signed<size_type>::type index_type;
    // at(), operator[]. Signed to support Python -ve indexes

    typedef typename boost::call_traits<value_type>::param_type value_param;
    typedef typename boost::call_traits<key_type>::param_type   key_param;
    typedef typename boost::call_traits<index_type>::param_type index_param;

    // *FIXME* should probably override the typedefs for iterator,
    // value_type and reference with the const versions if !is_mutable

    typedef value_traits<typename base_type::value_type> value_traits_;

    BOOST_STATIC_CONSTANT (bool,   has_mutable_ref
                           = base_type::has_mutable_ref && is_mutable);

    BOOST_STATIC_CONSTANT (bool,   has_find
                           = value_traits_::equality_comparable);

    // Assume the worst for everything else
    BOOST_STATIC_CONSTANT (bool,   has_insert         = false);
    BOOST_STATIC_CONSTANT (bool,   has_erase          = false);
    BOOST_STATIC_CONSTANT (bool,   has_pop_back       = false);
    BOOST_STATIC_CONSTANT (bool,   has_push_back      = false);

    // Forward visitor_helper to value_traits_
    template<typename PythonClass, typename Policy>
    static void visitor_helper (PythonClass &, Policy const &);
  };

  /////////////////////////////////////////////////////////////////////////
  // Traits for the iterator_pair container emulator
  /////////////////////////////////////////////////////////////////////////

  template<typename IteratorPair>
  struct iterator_pair_traits : public base_container_traits<IteratorPair>
  {
  };

  /////////////////////////////////////////////////////////////////////////
  // Default container traits - almost all "real" containers would meet
  // at least these requirements
  /////////////////////////////////////////////////////////////////////////

  template<typename Container>
  struct default_container_traits : public base_container_traits<Container>
  {
    BOOST_STATIC_CONSTANT (bool,   has_insert         = is_mutable);
    BOOST_STATIC_CONSTANT (bool,   has_erase          = is_mutable);
  };

  /////////////////////////////////////////////////////////////////////////
  // Sequences (list, deque, vector)
  /////////////////////////////////////////////////////////////////////////

  template<typename Container>
  struct default_sequence_traits : public default_container_traits<Container>
  {
    BOOST_STATIC_CONSTANT (bool,   has_pop_back       = is_mutable);
    BOOST_STATIC_CONSTANT (bool,   has_push_back      = is_mutable);
  };

  /////////////////////////////////////////////////////////////////////////
  // Sequences within a container_proxy
  /////////////////////////////////////////////////////////////////////////

  template<typename Container>
  struct container_proxy_traits : public default_sequence_traits<Container>
  {
    typedef Container container;
    typedef typename container::raw_value_type value_type; // insert, ...
    typedef typename container::raw_value_type key_type;   // find, count, ...
    typedef typename container::reference reference;       // return values

    typedef typename boost::call_traits<value_type>::param_type value_param;
    typedef typename boost::call_traits<key_type>::param_type   key_param;

    typedef value_traits<reference> value_traits_;
    // Get value_traits for the reference type (i.e. element_proxy)
    // to get the custom visitor_helper
  };

  /////////////////////////////////////////////////////////////////////////
  // Associative containers set and multiset
  /////////////////////////////////////////////////////////////////////////

  template<typename Container>
  struct set_traits : public default_container_traits<Container>
  {
    // *FIXME* handle const sets

    typedef void                            value_type; // index_type only (?)
    typedef typename Container::key_type    index_type; // operator[]
    typedef typename Container::key_type    key_type;   // find, count, ...

    typedef void *                                              value_param;
    typedef typename boost::call_traits<key_type>::param_type   key_param;
    typedef typename boost::call_traits<index_type>::param_type index_param;

    BOOST_STATIC_CONSTANT (IndexStyle,   index_style = index_style_nonlinear);
    BOOST_STATIC_CONSTANT (bool,   has_find    = true);
  };

  /////////////////////////////////////////////////////////////////////////
  // Associative containers map and multimap
  /////////////////////////////////////////////////////////////////////////

  template<typename Container>
  struct map_traits : public default_container_traits<Container>
  {
    // *FIXME* handle const maps
    typedef typename Container::mapped_type value_type;
    typedef value_type &                    reference;
    typedef typename Container::key_type    index_type; // operator[]
    typedef typename Container::key_type    key_type;   // find, count, ...

    typedef typename boost::call_traits<value_type>::param_type value_param;
    typedef typename boost::call_traits<key_type>::param_type   key_param;
    typedef typename boost::call_traits<index_type>::param_type index_param;

    BOOST_STATIC_CONSTANT (IndexStyle,   index_style = index_style_nonlinear);
    BOOST_STATIC_CONSTANT (bool,   has_find       = true);
    BOOST_STATIC_CONSTANT (bool,   is_reorderable = false);
  };
} } }

/////////////////////////////////////////////////////////////////////////
// Visitor helper function (foward to value_traits_ version)
/////////////////////////////////////////////////////////////////////////

template<typename Container>
template<typename PythonClass, typename Policy>
void
boost::python::indexing::base_container_traits<Container>
::visitor_helper (PythonClass &pyClass, Policy const &policy)
{
  value_traits_::visitor_helper (pyClass, policy);
}

#endif // container_suite_rmg_20030823_included