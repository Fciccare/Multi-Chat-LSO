package com.example.multichatlso.Model;

import androidx.annotation.NonNull;
import androidx.lifecycle.LifecycleOwner;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.Observer;

public class SingleLiveData<T> extends MutableLiveData<T> {
    @Override
    public void observe(@NonNull LifecycleOwner owner, @NonNull Observer<? super T> observer) {
        super.observe(owner, new Observer<T>() {
            @Override
            public void onChanged(T t) {
                if(t != null){
                    observer.onChanged(t);
                    setValue(null);
                }
            }
        });
        //removeObserver(observer);
    }

    public void singleObserve(@NonNull LifecycleOwner owner, @NonNull Observer<? super T> observer) {
        super.observe(owner, new Observer<T>() {
            @Override
            public void onChanged(T t) {
                if(t != null){
                    observer.onChanged(t);
                    setValue(null);
                }
            }
        });
        removeObserver(observer);
    }
}
