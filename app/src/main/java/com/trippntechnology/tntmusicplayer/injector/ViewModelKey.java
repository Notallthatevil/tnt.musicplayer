package com.trippntechnology.tntmusicplayer.injector;

import androidx.lifecycle.ViewModel;
import dagger.MapKey;

import java.lang.annotation.*;


//@MustBeDocumented
//@Target(AnnotationTarget.FUNCTION, AnnotationTarget.PROPERTY_GETTER, AnnotationTarget.PROPERTY_SETTER)
//@Retention(AnnotationRetention.RUNTIME)
//@MapKey
//annotation class ViewModelKey(val value: KClass<out ViewModel>)

@MapKey
@Documented
@Target({ ElementType.METHOD})
@Retention(RetentionPolicy.RUNTIME)
public @interface ViewModelKey {
    Class<? extends ViewModel> value();
}