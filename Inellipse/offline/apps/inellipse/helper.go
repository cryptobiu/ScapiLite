package main

import (
	"crypto/rand"
	"fmt"
	"io"
	"io/ioutil"
	"net/http"
	"os"
)

func DownloadFile(filepath string, url string) error {

	// Create the file
	out, err := os.Create(filepath)
	if err != nil {
		return err
	}

	defer out.Close()

	// Get the data
	resp, err := http.Get(url)
	if err != nil {
		return err
	}
	defer resp.Body.Close()

	// Write the body to file
	_, err = io.Copy(out, resp.Body)
	if err != nil {
		return err
	}

	return nil
}

func makeGetRequest(url string) string {
	// Make a get request
	rs, err := http.Get(url)
	// Process response
	if err != nil {
		panic(err) // More idiomatic way would be to print the error and die unless it's a serious error
	}
	defer rs.Body.Close()

	bodyBytes, err := ioutil.ReadAll(rs.Body)
	if err != nil {
		panic(err)
	}

	bodyString := string(bodyBytes)
	fmt.Println(bodyString)

	return bodyString
}

func generateRandomBytes(n int) ([]byte, error) {
	b := make([]byte, n)
	_, err := rand.Read(b)
	// Note that err == nil only if we read len(b) bytes.
	if err != nil {
		return nil, err
	}

	return b, nil
}

func generateRandomString() string {
	const letters = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-"
	bytes, err := generateRandomBytes(32)
	if err != nil {
		return ""
	}
	for i, b := range bytes {
		bytes[i] = letters[b%byte(len(letters))]
	}
	fmt.Println("simmetricKey:", string(bytes))
	return string(bytes)
}

func readFile(fileName string) string {
	dat, err := ioutil.ReadFile(fileName)
	check(err)
	// fmt.Println(string(dat))
	return string(dat)
}

func writeFile(fileBody string, fileName string) {
	d1 := []byte(fileBody)
	err := ioutil.WriteFile(fileName, d1, 0666)
	check(err)
	fmt.Println("File " + fileName + " is writed")
}

func check(e error) {
	if e != nil {
		panic(e)
	}
}
