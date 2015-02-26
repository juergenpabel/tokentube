extern tt_library_t*    g_library;

extern PyObject* py_tt_user_create(PyObject* self, PyObject *args);
extern PyObject* py_tt_user_update(PyObject* self, PyObject *args);
extern PyObject* py_tt_user_exists(PyObject* self, PyObject *args);
extern PyObject* py_tt_user_delete(PyObject* self, PyObject *args);
extern PyObject* py_tt_user_execute_verify(PyObject* self, PyObject *args);
extern PyObject* py_tt_user_execute_load(PyObject* self, PyObject *args);
extern PyObject* py_tt_user_execute_autoenrollment(PyObject* self, PyObject *args);

extern PyObject* py_tt_otp_create(PyObject* self, PyObject *args);
extern PyObject* py_tt_otp_exists(PyObject* self, PyObject *args);
extern PyObject* py_tt_otp_delete(PyObject* self, PyObject *args);
extern PyObject* py_tt_otp_execute_challenge(PyObject* self, PyObject *args);

