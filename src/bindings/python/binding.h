extern tt_library_t*    g_library;

extern PyObject* py_tt_user_create(PyObject* self, PyObject *args);
extern PyObject* py_tt_user_chpass(PyObject* self, PyObject *args);
extern PyObject* py_tt_user_key_add(PyObject* self, PyObject *args);
extern PyObject* py_tt_user_key_del(PyObject* self, PyObject *args);
extern PyObject* py_tt_user_delete(PyObject* self, PyObject *args);
extern PyObject* py_tt_user_exists(PyObject* self, PyObject *args);

extern PyObject* py_tt_otp_create(PyObject* self, PyObject *args);
extern PyObject* py_tt_otp_exists(PyObject* self, PyObject *args);
extern PyObject* py_tt_otp_delete(PyObject* self, PyObject *args);

extern PyObject* py_tt_auth_user_autoenrollment(PyObject* self, PyObject *args);
extern PyObject* py_tt_auth_user_verify(PyObject* self, PyObject *args);
extern PyObject* py_tt_auth_user_loadkey(PyObject* self, PyObject *args);
extern PyObject* py_tt_auth_otp_challenge(PyObject* self, PyObject *args);
extern PyObject* py_tt_auth_otp_response(PyObject* self, PyObject *args);
extern PyObject* py_tt_auth_otp_loadkey(PyObject* self, PyObject *args);


extern PyObject* py_tt_runtime_install_pba(PyObject* self, PyObject *args);

