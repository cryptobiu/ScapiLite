package com.inellipse.biumatrix.exception;

public class RecordNotFoundException extends RuntimeException {

    private static final long serialVersionUID = 2754169138632875096L;

    public RecordNotFoundException() {
        super();
    }

    public RecordNotFoundException(String message) {
        super(message);
    }

}
