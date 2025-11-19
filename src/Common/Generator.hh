#pragma once

#include <coroutine>
#include <exception>

/* Lazy generator functions.

Usage: Functions may be declared as `Generator<T>`. They should use the
keywords `co_yield` to yield values of type `T` (these are known as an 
intermediate suspend point), and `co_return` to end the generator, optionally
yielding a final value (this is known as the final suspend point.) 

The following design patterns help use the generator effectively:

- `while (gen)` or `for (int i=0; gen; i++)` to loop while there are more values
to generate; `gen()` to fetch the next value inside the loop. 
This takes advantage of the Generator's conversion operator to `bool`.

- `gen.next()` to generate the next value without fetching the previous value.
This is useful when only specific values are needed. The values can still be fetched
with `gen()` as and when required.*/
template<typename T>
struct Generator {

    struct promise_type {
        T current_value;
        std::exception_ptr exception;

        Generator get_return_object() {
            return Generator{
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }

        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }

        std::suspend_always yield_value(T&& value) {
            current_value = std::move(value);
            return {};
        }

        void unhandled_exception() {
            exception = std::current_exception();
        }

        void return_void() {}
    };
    using handle_type = std::coroutine_handle<promise_type>;
    handle_type coro;

    Generator(handle_type h) : coro(h) {}
    ~Generator() { if (coro) coro.destroy(); }

    /* Conversion operator to bool: returns `true` as long as there are more values to
    generate, allows use of the idiom `(if generator) { ... }`

    If the previous value has already been fetched, runs the coroutine to generate the 
    next value */
    explicit operator bool() {
        next_();
        return !coro.done();
    }

    /* Run the coroutine to generate the next value, then return it */
    T operator()() {
        next_();
        ready_ = false;
        return std::move(coro.promise().current_value);
    }

    /* Run the coroutine to generate the next value, regardless of whether the previous
    value has already been fetched */
    bool next() {
        ready_ = false;
        next_();
        return !coro.done();
    }

private:
    bool ready_ = false;
    /* Runs one more step of the coroutine to generate the next return value 
    
    `ready_` indicates whether there is a value ready to be fetched. */
    void next_() {
        if (!ready_) {
            coro.resume();
            if (coro.promise().exception) {
                std::rethrow_exception(coro.promise().exception);
            }
            ready_ = true;
        }
    }

};