# Laymen’s JS spec

While reading this, you are strongly advised to have a JS console opened, so you can try some stuff out for yourself. This document won't explain the syntactical oddities of JS, just the runtime semantics for someone looking to implement a JS runtime.

## 1. Value types

- Undefined and null - basically the same
- Booleans (true, false)
- Numbers (64 bit floats)
- Strings - characters are of a 16-bit width, and may contain \0
- Objects - a string to value dictionary, with a prototype (later on)
- Functions - an object that can be called, basically an object with a func ptr appended to it
- Arrays - Objects that have special behaviors when accessing number members, as well as “length”

## 2. Objects

### 2.1. Object basics

In layman's terms, a JS object is just a string to value hashmap. When we do a member lookup, we have to just convert the lookup key to a string and search for it in the hashmap (of course there's more to it, but let's get to it later).

NOTE: Keys are stored in order, so if you set "a", "b" and "c" in order, when we iterate the object's keys, we will get them in the same order - a, b and c. You can try it out in the console, by writing out the object literal `({ a: 10, b: 20, c: 30 })`. You can shuffle the properties around, and they will keep their order.

Objects define a few base operations for members:

- Member lookup (get the member's value)
- Member setting
- Member query (check if we have the member)
- Member removal
- Member definition (for now, it might look the same as member set, but we'll talk about it later)
- Member enumeration (in order)

### 2.2. Object prototypes

Prototypes are in the base of JS's OOP model. In short, it is a object that the lookup will "fallback" to, if we don't find the member in our object. Every JS object can have a prototype (some objects have what's called a `null` prototype). Since all object can have a prototype, we can have object A that has prototype B, and B can have prototype C. In this case, C can define "foo", B can define "bar" and A can define "baz". When we look up "foo" in A, the lookup will fallback on B, which will fallback on C, and then the member will be found.

```js
var A = { baz: 1 };
var B = { bar: 2 }
var C = { foo: 3 };

Object.setPrototypeOf(A, B);
Object.setPrototypeOf(B, C);

A.foo // 3
B.foo // 3
A.bar // 2
C.foo = 10;
A.foo // 10
B.foo // 10
```

Prototypes however make it impossible to know which object we're acting upon. This is why JS defines two types of member operations - normal and those, restricted to the current object (aka they don't work with the prototype). So with this consideration, our actual list of member operations is the following:

- Own member lookup
- Own member set
- Own member query
- Own member enumeration
- Own member removal
- Own member definition (again, we will get to the difference from `set`)
- Normal member lookup
- Normal member set
- Normal member query
- Normal member enumeration

Note how we can ony define and remove members from the specific object. In short, this is because we almost never want to delete a member from the prototype if we don't find it in the object itself. This is because a prototype may be used by thousands of objects, and if one of them accidentally deletes a property of the prototype, it will break the rest. The same logic follows for member definition

Additionally, we can define three more operations with the prototype:

- Prototype get
- Prototype set
- Prototype remove

### 2.3. Properties and fields

Until now, we've looked at just members, which were just JS values. However, the JS model is far more flexible. First of all, we have two types of members: fields and properties.

Fields are the simpler member - they are just a value, with the following additional options:
- writable - if false, sets to this member will not have an effect
- configurable - we'll get to it
- enumerable - if false, the enumeration will skip over this member by default

Properties are the more complex counterpart of members - when we try to get them, a user-defined JS function will be called, and its return value will be returned as the member's value. Similarly, a user-defined JS function with one argument is called when setting the value of the member. Those two functions - getters and setters, are optionally defined (aka a user may only define the getter, making the property effectively readonly)

Properties have the same flags as fields, except for writable - as was said above, the setter can just be omitted.

### 2.4. Member configuration (redefinition) and deletion

Since members have the above mentioned flags, we can "tweak" them under certain circumstances. This is called a configuration. The act of redefining or deleting a member, to be precise, is a configuration.

As you might've spotted, we have a "configurable" flag. This flag is used to "lock down" the member, preventing people from redefining it. After setting "configurable" to false, we can no longer delete or redefine the member. In fact, just one redefinition is allowed for non-configurable members - if the member is a field, and it is writable, we are allowed to redefine it with a field with the same value, enumerable and configurable flag, but with "writable" set to false.

Otherwise, if we try to redefine a non-enumerable member with the exact same parameters, the operation will succeed, and nothing will change.

Another caveat is that, when a configurable property is redefined, instead of straight up overwritting it, if the property has a null getter or setter, the respective function is going to be taken from the old object:
```js
var obj = {};
Object.defineProperty(obj, "test", { get: () => 10, enumerable: true, configurable: true });
/*
{
	get: [Function],
	set: undefined,
	configurable: true,
	enumerable: true,
}
*/
Object.defineProperty(obj, "test", { set: v => console.log(v), enumerable: false, configurable: true });
Object.getOwnMemberDescriptor(obj, "test");
/*
{
	get: [Function],
	set: [Function],
	configurable: true,
	enumerable: false,
}
*/
```

The pseudocode for redefining a field is as follows:
```js
function redefine(old, new) {
	if (old.configurable) {
		if (old is Property) {
			old.get = new.get ?? old.get;
			old.set = new.set ?? old.set;
		}
		old = new;
		return true;
	}
	else {
		if (old.enumerable != new.enumerable) return false;
		if (new.configurable) return false;

		if (old is Field) {
			if (!old.writable) {
				if (old.value != new.value) return false;
				if (new.writable) return false;
			}
			else old = new;

			return true;
		}
		else (old is Property) {
			if (old.get != new.get) return false;
			if (old.set != new.set) return false;

			return true;
		}
	}
}
```

Deletion is much more straight-forward - we only ought to check if the member exists and is configurable. Note that a deletion will succeed if the member doesn't exist - it will fail only if the member isn't configurable.

### 2.5. Normal, non-extensible, sealed and frozen objects

These are the four states of a JS object, which will determine to what extent we can modify it.

1. Normal mode - all the so far explained semantics are the same
2. Non-extensible mode, we can't define new members, but we are allowed to redefine and delete them. The prototype of the object is also locked as-is
3. Sealed - Same as non-extensible, but all the members are non-configurable, too (can't delete them and can't redefine them, except for the mentioned mechanics)
4. Frozen - Same as non-extensible, but all members are non-configurable and all fields are readonly. Properties retain their setters

Note that a runtime may implement only a extensible flag on the object, and the rest may be done from JS instead.

## 3. Arrays

In short, arrays are nothing more than special-case objects. All arrays have the "length" member, as well as members with numeric keys, ranging from 0 to length. These keys are arbitrary, and it's just a "contract" of sorts that they will represent elements from an array. JS arrays are sparse as well. This means that a JS array may define elements from 0 to 5, and then from 8 to 10. In this case, elements 6 to 7 are not members of the array (Try it out by writing the `[1,,2]` literal in your terminal. You will see `[1, <empty>, 2]`, and when you try to call `Object.getOwnPropertyDescriptor([1,,2], "1")`, you will get `undefined`, aka no such field). Another caveat is that when you define the member "5" of an array of size "3", the array's length will automagically update to `6` (because index `5` is the 6th element). Setting the "length" property of the array can have two effects: if the new size is bigger, nothing (the array will just grow, and will be filled by empty values), and if the size is smaller, the array will automagically shrink to that size, deleting the members that are outside the bounds of the array.

Most sane engines of course define a special case object for arrays, which are usually backed by a linear buffer. When you try to grow the array by setting members out of its bounds, it will grow its buffer to accommodate its new members. When deleting a member (which will make the array sparse), engines usually take one of two routes: they either enter panic mode and convert the whole array to a normal object, or just set a special "flag" value in place of the empty value.

Other nasty things the user can do are: define a normal member (for example "pe6o") in the array, defining a property with the name "0" in an array  and defining the member "0.5". The first two are handled by just having an underlying object on speed dial. The last example however is more interesting - in such cases, most engines will either, again, enter panic mode and revert to object mode, or just overlay the array with a backing object.
