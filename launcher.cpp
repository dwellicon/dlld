#include <windows.h>
#include <jni.h>
#include <string>

// --- FINAL HARDCODED PATHS ---
const char* JVM_DLL = "C:\\Users\\khuang0083\\Downloads\\jvm.dll";
const char* JAR_FILE = "C:\\Users\\khuang0083\\Downloads\\ATLauncher.jar";
// Standard main class for ATLauncher
const char* MAIN_CLASS = "com/atlauncher/App"; 

typedef jint (JNICALL *CREATE_JVM)(JavaVM**, void**, void*);

extern "C" __declspec(dllexport) void CALLBACK RunJava(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow) {
    // 1. Load jvm.dll directly from your Downloads
    HMODULE hJvm = LoadLibraryA(JVM_DLL);
    if (!hJvm) {
        MessageBoxA(NULL, "Could not load jvm.dll from Downloads folder.", "Error", MB_ICONERROR);
        return;
    }

    // 2. Find the entry point to create the Virtual Machine
    CREATE_JVM JNI_CreateJavaVM_Ptr = (CREATE_JVM)GetProcAddress(hJvm, "JNI_CreateJavaVM");
    if (!JNI_CreateJavaVM_Ptr) return;

    JavaVM *jvm;
    JNIEnv *env;
    JavaVMInitArgs vm_args;
    JavaVMOption options[1];

    // 3. Set the classpath to point to the ATLauncher JAR
    std::string cpArg = "-Djava.class.path=" + std::string(JAR_FILE);
    options[0].optionString = (char*)cpArg.c_str();

    vm_args.version = JNI_VERSION_1_8;
    vm_args.nOptions = 1;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = JNI_FALSE;

    // 4. Start the JVM
    if (JNI_CreateJavaVM_Ptr(&jvm, (void**)&env, &vm_args) == JNI_OK) {
        jclass cls = env->FindClass(MAIN_CLASS);
        if (cls) {
            jmethodID mid = env->GetStaticMethodID(cls, "main", "([Ljava/lang/String;)V");
            if (mid) {
                // Create empty string arguments for the main method
                jobjectArray args = env->NewObjectArray(0, env->FindClass("java/lang/String"), NULL);
                env->CallStaticVoidMethod(cls, mid, args);
            }
        }
        // Cleanup
        jvm->DestroyJavaVM();
    }
    FreeLibrary(hJvm);
}
