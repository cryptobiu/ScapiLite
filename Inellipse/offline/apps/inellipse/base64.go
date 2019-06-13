package main

import (
	"encoding/base64"
	"fmt"
)

func base64Encode(message string) (retour string) {
	data := []byte(message)
	base64Text := base64.StdEncoding.EncodeToString(data)
	// base64Text := make([]byte, base64.StdEncoding.EncodedLen(len(message)))
	// base64.StdEncoding.Encode(base64Text, []byte(message))
	fmt.Println(string(base64Text))
	base64Decode(string(base64Text))
	return string(base64Text)
}

func base64Decode(str string) string {
	sDec, _ := base64.StdEncoding.DecodeString(str)
	fmt.Println("-------------------------------")
	fmt.Println(string(sDec))
	fmt.Println("-------------------------------")

	return string(sDec)
}
