#ifndef RADAPTER_UTILITY_CLASSINATOR_HPP
#define RADAPTER_UTILITY_CLASSINATOR_HPP

#include <radapter/Utility/CallbackInfo.hpp>

namespace radapter
{
    /**
     * A collection of static helper methods to generate and manipulate node
     * classes.
     *
     * T is the radapter class describing how its corresponding node class
     * should be instantiated. U is the wrapped Randar class.
     */
    template <class T, class U>
    struct Classinator
    {
        /**
         * Generates the constructor function for the class.
         *
         * This does not immediately generate an instance of the class. Rather,
         * it provides a Node construction function which creates the instance
         * once invoked.
         */
        static napi_value constructor(napi_env env)
        {
            std::vector<napi_property_descriptor> properties = T::properties();

            napi_value fn;
            radapter::checkNapi("define node constructor", env, napi_define_class(
                env,
                T::className.c_str(),
                Classinator<T, U>::instance,
                nullptr,
                properties.size(),
                properties.data(),
                &fn
            ));

            return fn;
        }

        /**
         * Callback that cleans up a class instance upon garbage collection.
         */
        static void finalize(napi_env env, void* data, void* hint)
        {
            delete reinterpret_cast<U*>(data);
        }

        /**
         * Callback of the constructor that creates an instance of the class.
         *
         * The constructor first creates the actual node object before invoking
         * this. It is provided as the "this" argument in the callback info,
         * available here as info.self. We wrap the true Randar class within the
         * object node instantiated then pass it through.
         */
        static napi_value instance(napi_env env, napi_callback_info info)
        {
            radapter::CallbackInfo cbInfo(env, info);
            Classinator<T, U>::wrap(env, cbInfo.self, T::instance(cbInfo));
            return cbInfo.self;
        }

        /**
         * Wraps a JavaScript object around a native one.
         */
        static void wrap(napi_env env, napi_value jsValue, U* nativeValue)
        {
            napi_ref ref;
            radapter::checkNapi("wrap native object", env, napi_wrap(
                env,
                jsValue,
                nativeValue,
                Classinator<T, U>::finalize,
                nativeValue,
                &ref
            ));
        }
    };
}

#endif
