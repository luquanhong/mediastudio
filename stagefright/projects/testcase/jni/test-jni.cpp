/*!
 *	\file jni adapter source
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <jni.h> 

#include "test.h"
#include "um_video_dec.h"


/*!
*	\brief	pass the surface to native from java, and relationship with windows
*/
struct fields_t {
    jfieldID    surface;	//!< actually in android.view.Surface XXX 
    jfieldID    surface_native;	//!< the surface passed from java
}; 
static fields_t fields;

VDCtx* ctx;

/*!
*	\brief simple test case
*/
JNIEXPORT jstring JNICALL Java_com_example_hellojni_HelloJni_stringFromJNI(JNIEnv * env, jobject obj)
{
	//return env->NewStringUTF( (char *)"HelloJni Native String");
	return 0;
}

/*!
*	\brief simple test case
*/
JNIEXPORT jint JNICALL jni_test_case(JNIEnv * env, jobject obj)
{
	LOGE("test");
	//return test_case();
	return 0;
		
}

/*!
*	\brief circle list test case
*/
JNIEXPORT jint JNICALL jni_test_circle_list(JNIEnv * env, jobject obj)
{
	LOGD("Hello jni_test_circle_list!");
	//return test_circle_list();
	return 0;	
}

/*!
*	\brief	simple video decode test case. it will invoke the functions that location in the test.cpp.
*			it can only test the interface. 
*/
JNIEXPORT jint JNICALL jni_test_video_dec(JNIEnv * env, jobject obj)
{
	LOGE("Hello jni_test_video_dec!");
	//return test_video_dec();
	return 0;	
}

/*!
*	\brief	simple interface invoke without implement
*/
JNIEXPORT jint JNICALL jni_um_vdec_init(JNIEnv * env, jobject obj)
{
	return  um_vdec_init();

}

/*!
*	\brief	set the params for video player 
*/
JNIEXPORT jint JNICALL jni_um_vdec_create(JNIEnv * env, jobject obj)
{
	LOGE("Hello jni_um_vdec_create! type is ");
	CodecParams* params;
	
	params = (CodecParams *)malloc(sizeof(struct _CodecParams));	
	if(!params)		
		LOGE("params memory error");
	
	params->frameType= FRAME_TYPE_RGB;//h264
//	params->frameType= FRAME_TYPE_YUV;//mpeg-4
	
	params->v.width = 800;	
	params->v.height = 480;

	LOGE("Hello jni_um_vdec_create! type is ");
	ctx = um_vdec_create(params);
}

/*!
*	\brief	static function, it will be invoke by jni_um_video_surface_set
*/
static void* get_surface(JNIEnv* env, jobject clazz)
{

    void* p = (void* )env->GetIntField(clazz, fields.surface_native);
    return (p);

}

/*!
*	\brief	relationship surface with native window,the surface is passed from java
*/
JNIEXPORT jint JNICALL jni_um_vdec_setSurface(JNIEnv * env, jobject obj)
{
	LOGE("Hello jni_um_video_surface_set!");
	
	jobject surface = env->GetObjectField(obj, fields.surface);
	LOGE("jni_um_vdec_setSurface(), surface=%p", surface);
	if (surface != NULL) 
	{
		void* native_surface = get_surface(env, surface);
				return um_vdec_setSurface(ctx, native_surface);
    }
	
	return -1;
}


/*!
*	\brief	main decode function,
*	\param	in	it's nal buf
*	\param	len	it's the length of nal
*	\return	if success, it will return UM_SUCCESS
*/
JNIEXPORT jint JNICALL jni_um_vdec_decode(JNIEnv * env, jobject obj,jbyteArray in, jint len)
{

	UMSint ret;

	jbyte * buf = (jbyte*)env->GetByteArrayElements(in, 0);

	LOGE("buf is 0x%x  0x%x 0x%x 0x%x and length  is %d",buf[0],buf[1],buf[2],buf[4],len);
	ret = um_vdec_decode(ctx, buf, len);

	env->ReleaseByteArrayElements(in, buf, 0); 
	
	if(ret == -1)	
	{		
		LOGE("um_vdec_decode  fail\n");		
		return -3;	
	}
	
}

/*!
*	\brief	simple interface invoke without implement
*/
JNIEXPORT jint JNICALL jni_um_vdec_destroy(JNIEnv * env, jobject obj)
{
	return um_vdec_destroy(ctx); 

}

/*!
*	\brief	simple interface invoke without implement
*/
JNIEXPORT jint JNICALL jni_um_vdec_fini(JNIEnv * env, jobject obj)
{
	return um_vdec_fini();
}

/*!
*	\brief	funciton table from java to native 
*/
static JNINativeMethod methods[] = {
                {"stringFromJNI",		"()Ljava/lang/String;", 	(void*)Java_com_example_hellojni_HelloJni_stringFromJNI },
				{"um_vdec_init",		"()I",						(void*)jni_um_vdec_init},
				{"um_vdec_create",		"()I",						(void*)jni_um_vdec_create},
				{"um_vdec_decode",		"([BI)I",					(void*)jni_um_vdec_decode},
				{"um_vdec_destroy",		"()I",						(void*)jni_um_vdec_destroy},
				{"um_vdec_fini",		"()I",						(void*)jni_um_vdec_fini},
				{"um_vdec_setSurface","()I",						(void*)jni_um_vdec_setSurface},
				
};

/*!
 * 	\brief	The Java layout invoke class
 */
static const char *classPathName = "com/example/testcase/TestCaseActivity";

/*!
 * 	\brief	Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
                                JNINativeMethod* gMethods, int numMethods)
{
	jclass clazz;
	fprintf(stderr, "RegisterNatives start for '%s'", className);
	clazz = env->FindClass(className);	//!< get java class instance from my java class
	if (clazz == NULL) {
			fprintf(stderr, "Native registration unable to find class '%s'", className);
			return JNI_FALSE;
	}
	
	fields.surface = env->GetFieldID(clazz, "mSurface", "Landroid/view/Surface;");	//!< get the class of surface var in the java class
    if (fields.surface == NULL) {
        LOGE("Can't find GameCloudPlayer.mSurface");
        return -1;
    }

    jclass surface = env->FindClass("android/view/Surface");	//!< get java class instance from system class
    if (surface == NULL) {
        LOGE("Can't find android/view/Surface");
        return -1;
    }
	fields.surface_native = env->GetFieldID(surface, "mNativeSurface", "I");	//!< get the no of mNativeSurface var in the system 
	
	
	if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
			fprintf(stderr, "RegisterNatives failed for '%s'", className);
			return JNI_FALSE;
	}

	return JNI_TRUE;
}

/*!
 * 	\brief Register native methods for all classes we know about.
 * 	\return JNI_TRUE on success.
 */
static int registerNatives(JNIEnv* env)
{
        if (!registerNativeMethods(env, classPathName,
                methods, sizeof(methods) / sizeof(methods[0]))) {
                        return JNI_FALSE;
        }

        return JNI_TRUE;
}


/*!
 * 	\brief switch c and c++
 */
typedef union {
        JNIEnv* env;
        void* venv;
} UnionJNIEnvToVoid;

/*!
 * 	\brief This is called by the VM when the shared library is first loaded.
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
        UnionJNIEnvToVoid uenv;
        uenv.venv = NULL;
        jint result = -1;
        JNIEnv* env = NULL;

        printf("JNI_OnLoad");

        if (vm->GetEnv(&uenv.venv, JNI_VERSION_1_4) != JNI_OK) {
                fprintf(stderr, "GetEnv failed");
                goto bail;
        }

        env = uenv.env;

        if (registerNatives(env) != JNI_TRUE) {
                fprintf(stderr, "GetEnv failed");
                goto bail;
}

                result = JNI_VERSION_1_4;
bail:
    return result;
}

