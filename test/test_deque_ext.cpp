// -*- mode:c++ -*-
//
// Module test_deque_ext.cpp
//
// Copyright (c) 2003 Raoul M. Gough
//
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy
// at http://www.boost.org/LICENSE_1_0.txt)
//
// History
// =======
// 2003/10/15   rmg     File creation from testlinear.cpp
//
// $Id$
//

#include "int_wrapper.hpp"

#include <boost/python/suite/indexing/container_suite.hpp>
#include <boost/python/suite/indexing/container_proxy.hpp>
#include <deque>
#include <boost/python/class.hpp>
#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/implicit.hpp>

// More messiness from not having a separate int_wrapper.cpp file
bool int_wrapper::our_trace_flag = true;
unsigned int_wrapper::our_object_counter = 0;

BOOST_PYTHON_MODULE(test_deque_ext)
{
  boost::python::implicitly_convertible <int, int_wrapper>();

  boost::python::def ("setTrace", &int_wrapper::setTrace);

  boost::python::class_<int_wrapper>("int_wrapper", boost::python::init<int>())
    .def ("increment", &int_wrapper::increment)
    .def ("__repr__", &int_wrapper::repr)
    .def ("__cmp__", compare)
    ;

  typedef std::deque<int> Container1;

  boost::python::class_<Container1>("Deque")
    .def (boost::python::indexing::container_suite<Container1>())
    ;

  typedef std::deque<int_wrapper> Container2;

  // Returning internal references to elements of a deque is
  // dangerous - the references can be invalidated by inserts or
  // deletes!
  boost::python::class_<Container2>("Deque_ref")
    .def (boost::python::indexing::container_suite<Container2>
          ::with_policies (boost::python::return_internal_reference<>()));

  typedef boost::python::indexing::container_proxy< std::deque<int_wrapper> >
    Container3;

  boost::python::class_<Container3>("Deque_proxy")
    .def (boost::python::indexing::container_suite<Container3>())
    ;
}