#pragma once

#include "munit/munit.h"

MunitResult test_new_task(const MunitParameter params[],
                          void *user_data_or_fixture);
MunitResult test_edit_task(const MunitParameter params[],
                           void *user_data_or_fixture);
MunitResult test_task_repr(const MunitParameter params[],
                           void *user_data_or_fixture);

MunitResult test_new_task_node(const MunitParameter params[],
                               void *user_data_or_fixture);
MunitResult test_new_chain(const MunitParameter params[],
                           void *user_data_or_fixture);
MunitResult test_add_task(const MunitParameter params[],
                          void *user_data_or_fixture);
MunitResult test_delete_task(const MunitParameter params[],
                             void *user_data_or_fixture);
MunitResult test_find_task(const MunitParameter params[],
                           void *user_data_or_fixture);

MunitResult test_serialize_task(const MunitParameter params[],
                                void *user_data_or_fixture);

MunitResult test_deserialize_task(const MunitParameter params[],
                                  void *user_data_or_fixture);

MunitResult test_serialize_chain(const MunitParameter params[],
                                 void *user_data_or_fixture);
MunitResult test_deserialize_chain(const MunitParameter params[],
                                   void *user_data_or_fixture);
