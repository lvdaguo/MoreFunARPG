#pragma once

#define BB_SET_BOOL(KeyName, Value) \
	GetBlackboardComponent()->SetValueAsBool(KeyName, Value);

#define BB_GET_BOOL(KeyName) \
	(GetBlackboardComponent()->GetValueAsBool(KeyName))

#define BB_SET_OBJECT(KeyName, Value) \
	GetBlackboardComponent()->SetValueAsObject(KeyName, Value);

#define BB_GET_OBJECT(KeyName) \
	(GetBlackboardComponent()->GetValueAsObject(KeyName))

#define BB_SET_VECTOR(KeyName, Value) \
	GetBlackboardComponent()->SetValueAsVector(KeyName, Value);

#define BB_GET_VECTOR(KeyName) \
	(GetBlackboardComponent()->GetValueAsVector(KeyName))