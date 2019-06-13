package com.inellipse.biumatrix.exception;

public class BadRequestException extends RuntimeException {

    private static final long serialVersionUID = 2754169138632875096L;

    public BadRequestException() {
        super();
    }

    public BadRequestException(String message) {
        super(message);
    }

}
