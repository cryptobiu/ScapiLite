package crypto.cs.biu.scapilite.util;

import android.util.Base64;

import org.spongycastle.asn1.ASN1Encodable;
import org.spongycastle.asn1.ASN1Primitive;
import org.spongycastle.asn1.pkcs.PrivateKeyInfo;
import org.spongycastle.asn1.x509.SubjectPublicKeyInfo;
import org.spongycastle.jce.provider.BouncyCastleProvider;
import org.spongycastle.openssl.PEMKeyPair;
import org.spongycastle.openssl.PEMParser;
import org.spongycastle.openssl.jcajce.JcaPEMKeyConverter;
import org.spongycastle.util.io.pem.PemObject;
import org.spongycastle.util.io.pem.PemWriter;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;
import java.io.StringWriter;
import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.security.InvalidAlgorithmParameterException;
import java.security.InvalidKeyException;
import java.security.Key;
import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.Security;
import java.security.spec.InvalidKeySpecException;
import java.security.spec.InvalidParameterSpecException;
import java.security.spec.KeySpec;
import java.security.spec.MGF1ParameterSpec;
import java.security.spec.PKCS8EncodedKeySpec;
import java.security.spec.X509EncodedKeySpec;

import javax.crypto.BadPaddingException;
import javax.crypto.Cipher;
import javax.crypto.IllegalBlockSizeException;
import javax.crypto.KeyGenerator;
import javax.crypto.NoSuchPaddingException;
import javax.crypto.SecretKey;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.OAEPParameterSpec;
import javax.crypto.spec.PSource;
import javax.crypto.spec.SecretKeySpec;

import static crypto.cs.biu.scapilite.util.Logger.log;
import static crypto.cs.biu.scapilite.util.Logger.logError;


public class SecurityHelper
{
    public static void generatePrivateAndPublicKeys(String trans)
    {
        if (PreferencesManager.getPublicPem() == null || PreferencesManager.getPublicPem().equals(""))
        {
            try
            {
                generateKeyPair(trans);
            }
            catch (Exception e)
            {
                logError("pemString3 ", e);
            }
        }
        log("2222 encryptToRSAString start");
        String enc = encryptToRSAString("vlade,aren???", PreferencesManager.getPublic(), trans);
        log("2222 encryptToRSAString " + enc);
        String decr = decryptRSAToString(enc, PreferencesManager.getPrivate(), trans);
        log("2222 decryptRSAToString ->" + decr);
    }

    public static void generateKeyPair(String trans) throws NoSuchProviderException, NoSuchAlgorithmException, IOException
    {
        int KEY_SIZE = 1024;
        log("pemString");

        Security.addProvider(new BouncyCastleProvider());

        KeyPairGenerator keygen = KeyPairGenerator.getInstance(trans, "BC");
        keygen.initialize(KEY_SIZE);
        KeyPair pair = keygen.generateKeyPair();


        PublicKey publicKey = pair.getPublic();
        PrivateKey privateKey = pair.getPrivate();

        log("1111 publicKey alg " + publicKey.getAlgorithm());
        log("1111 publicKey getFormat " + publicKey.getFormat());
        log("1111 publicKey getEncoded().length " + publicKey.getEncoded().length);
        log("1111 privateKey alg " + privateKey.getAlgorithm());
        log("1111 privateKey getFormat " + privateKey.getFormat());
        log("1111 privateKey getEncoded().length " + privateKey.getEncoded().length);

        byte[] publicKeyBytes = publicKey.getEncoded();
        String pubKeyStr = new String(Base64.encode(publicKeyBytes, Base64.DEFAULT));

        byte[] privKeyBytes = privateKey.getEncoded();
        String privKeyStr = new String(Base64.encode(privKeyBytes, Base64.DEFAULT));

        PreferencesManager.putPublic(pubKeyStr);
        PreferencesManager.putPrivate(privKeyStr);

        PrivateKeyInfo pkInfo = PrivateKeyInfo.getInstance(privKeyBytes);
        ASN1Encodable encodable = pkInfo.parsePrivateKey();
        ASN1Primitive primitive = encodable.toASN1Primitive();
        byte[] privateKeyPKCS1 = primitive.getEncoded();

        PemObject pemObject = new PemObject("RSA PRIVATE KEY", privateKeyPKCS1);
        StringWriter stringWriter = new StringWriter();
        PemWriter pemWriter = new PemWriter(stringWriter);
        pemWriter.writeObject(pemObject);
        pemWriter.close();
        String pemString = stringWriter.toString();
        log("pemString1");
        log(pemString);
        PreferencesManager.putPrivatePem(pemString);

        SubjectPublicKeyInfo spkInfo = SubjectPublicKeyInfo.getInstance(publicKeyBytes);
        primitive = spkInfo.parsePublicKey();
        byte[] publicKeyPKCS1 = primitive.getEncoded();

        pemObject = new PemObject("RSA PUBLIC KEY", publicKeyPKCS1);
        stringWriter = new StringWriter();
        pemWriter = new PemWriter(stringWriter);
        pemWriter.writeObject(pemObject);
        pemWriter.close();
        pemString = stringWriter.toString();

        log("pemString2");
        log(pemString);
        PreferencesManager.putPublicPem(pemString);

    }


    public static String encryptToRSAString(String clearText, String publicKey, String trans)
    {
        String encryptedBase64 = "";
        try
        {
            KeyFactory keyFac = KeyFactory.getInstance("RSA");
//            KeyFactory keyFac = KeyFactory.getInstance(trans);
            KeySpec keySpec = new X509EncodedKeySpec(Base64.decode(publicKey.trim().getBytes(), Base64.DEFAULT));
            Key key = keyFac.generatePublic(keySpec);
            // get an RSA cipher object and print the provider
            final Cipher cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding");
            // encrypt the plain text using the public key
            cipher.init(Cipher.ENCRYPT_MODE, key);
            byte[] encryptedBytes = cipher.doFinal(clearText.getBytes("UTF-8"));
            encryptedBase64 = new String(Base64.encode(encryptedBytes, Base64.DEFAULT));
            log("encryptToRSAString " + encryptedBase64);
        }
        catch (Exception e)
        {
            logError("encryptToRSAString ", e);
        }
        return encryptedBase64.replaceAll("(\\r|\\n)", "");
    }


    public static String decryptRSAToString(String encryptedBase64, String privateKey, String trans)
    {
        //        '/' and '+' to '_' and '-'
//        encryptedBase64 = encryptedBase64.replace("/", "_");
//        encryptedBase64 = encryptedBase64.replace("+", "-");

//        log("decryptRSAToString privateKey" + privateKey);

        String decryptedString = "N/A";
        try
        {
            KeyFactory keyFac = KeyFactory.getInstance("RSA");
//            KeyFactory keyFac = KeyFactory.getInstance(trans);
            KeySpec keySpec = new PKCS8EncodedKeySpec(Base64.decode(privateKey.trim().getBytes(), Base64.DEFAULT));
            Key key = keyFac.generatePrivate(keySpec);

            // get an RSA cipher object and print the provider
//            final Cipher cipher = Cipher.getInstance("RSA/ECB/OAEPWithSHA-256AndMGF1Padding");
//            final Cipher cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding");
//            final Cipher cipher = Cipher.getInstance("RSA/ECB/OAEPwithSHA1andMGF1Padding");


            // encrypt the plain text using the public key
//            OAEPParameterSpec oaepParams = new OAEPParameterSpec("SHA-256", "MGF1", new MGF1ParameterSpec("SHA-256"), PSource.PSpecified.DEFAULT);
//            cipher.init(Cipher.DECRYPT_MODE, key, oaepParams);


            // 1 - javax.crypto.BadPaddingException: data hash wrong at com.android.org.bouncycastle.jcajce.provider.asymmetric.rsa.CipherSpi.engineDoFinal(CipherSpi.java:479)
            //   - with pem - java.security.spec.InvalidKeySpecException: java.lang.RuntimeException: error:0c0000b9:ASN.1 encoding routines:OPENSSL_internal:WRONG_TAG
            final Cipher cipher = Cipher.getInstance("RSA/ECB/OAEPPadding");
            OAEPParameterSpec oaepParams = new OAEPParameterSpec("SHA-256", "MGF1", new MGF1ParameterSpec("SHA-256"), PSource.PSpecified.DEFAULT);
            cipher.init(Cipher.DECRYPT_MODE, key, oaepParams);
            byte[] encryptedBytes = Base64.decode(encryptedBase64, Base64.DEFAULT);


            byte[] decryptedBytes = cipher.doFinal(encryptedBytes);
            decryptedString = new String(decryptedBytes);
            log("decryptRSAToString " + decryptedString);
        }
        catch (Exception e)
        {
            logError("decryptRSAToString e ", e);
        }

        return decryptedString;
    }


    public static void decryptString(String base64String, String privateKey)
    {
        try
        {
//        FileInputStream is = new FileInputStream("priv.p12");
//        KeyStore keystore = KeyStore.getInstance("PKCS12");
//        keystore.load(is, "".toCharArray());
//        System.out.println("Successfully loaded");


            String keyAlias = "1";

//        PrivateKey key = (PrivateKey) keystore.getKey(keyAlias, "".toCharArray());

//        System.out.println("key "+Base64.encodeBase64String(key.getEncoded()));
            Cipher rsaDecryptCipher;
            rsaDecryptCipher = Cipher.getInstance("RSA/ECB/OAEPWITHSHA-256ANDMGF1PADDING");
            rsaDecryptCipher.init(Cipher.DECRYPT_MODE, stringToPrivateKey(privateKey));
            final byte[] plainText = rsaDecryptCipher.doFinal(Base64.decode(base64String, Base64.DEFAULT));

            log("decryptRSAToString2 : " + new String(plainText));

        }
        catch (Exception e)
        {
            logError("decryptRSAToString2 e ", e);
        }
    }

    public static PrivateKey stringToPrivateKey(String keyString) throws InvalidKeySpecException, NoSuchAlgorithmException, IOException
    {
        StringBuilder pkcs8Lines = new StringBuilder();
        BufferedReader rdr = new BufferedReader(new StringReader(keyString));
        String line;
        while ((line = rdr.readLine()) != null)
        {
            pkcs8Lines.append(line);
        }

        // Remove the "BEGIN" and "END" lines, as well as any whitespace

        String pkcs8Pem = pkcs8Lines.toString();
        pkcs8Pem = pkcs8Pem.replace("-----BEGIN PRIVATE KEY-----", "");
        pkcs8Pem = pkcs8Pem.replace("-----END PRIVATE KEY-----", "");
        pkcs8Pem = pkcs8Pem.replaceAll("\\s+", "");

        // Base64 decode the result

        byte[] pkcs8EncodedBytes = Base64.decode(pkcs8Pem, Base64.DEFAULT);

        // extract the private key

        PKCS8EncodedKeySpec keySpec = new PKCS8EncodedKeySpec(pkcs8EncodedBytes);
        KeyFactory kf = KeyFactory.getInstance("RSA");
        PrivateKey privKey = kf.generatePrivate(keySpec);
        return privKey;
    }

    public static PublicKey stringToPublicKey() throws InvalidKeySpecException, NoSuchAlgorithmException, IOException
    {
        StringBuilder pkcs8Lines = new StringBuilder();
        BufferedReader rdr = new BufferedReader(new StringReader(PreferencesManager.getPublicPem()));
        String line;
        while ((line = rdr.readLine()) != null)
        {
            pkcs8Lines.append(line);
        }

        // Remove the "BEGIN" and "END" lines, as well as any whitespace

        String pkcs8Pem = pkcs8Lines.toString();
        pkcs8Pem = pkcs8Pem.replace("-----BEGIN PUBLIC KEY-----", "");
        pkcs8Pem = pkcs8Pem.replace("-----END PUBLIC KEY-----", "");
        pkcs8Pem = pkcs8Pem.replaceAll("\\s+", "");

        // Base64 decode the result

        byte[] pkcs8EncodedBytes = Base64.decode(pkcs8Pem, Base64.DEFAULT);

        // extract the private key

        PKCS8EncodedKeySpec keySpec = new PKCS8EncodedKeySpec(pkcs8EncodedBytes);
        KeyFactory kf = KeyFactory.getInstance("RSA");
        PublicKey pubKey = kf.generatePublic(keySpec);
        return pubKey;
    }


//    private static KeyPair getKeyPair()
//    {
//        KeyPair kp = null;
//        try
//        {
//            KeyPairGenerator kpg = KeyPairGenerator.getInstance("RSA");
//            kpg.initialize(512);
//            kp = kpg.generateKeyPair();
//        }
//        catch (Exception e)
//        {
//            e.printStackTrace();
//        }
//
//        return kp;
//    }

//    private void signCert(KeyPair publicKey)
//    {
//        log("x509CertificateHolder. signCert:  ");
//        Date startDate = new Date(System.currentTimeMillis() - 24 * 60 * 60 * 1000);
//        Date endDate = new Date(System.currentTimeMillis() + 30 * 24 * 60 * 60 * 1000);
//
//        X500NameBuilder nameBuilder = new X500NameBuilder(BCStyle.INSTANCE);
//        nameBuilder.addRDN(BCStyle.O, "testO");
//        nameBuilder.addRDN(BCStyle.OU, "testOU");
//        nameBuilder.addRDN(BCStyle.L, "testL");
//
//        X500Name x500Name = nameBuilder.build();
//        Random random = new Random();
//        log("x509CertificateHolder. signCert:2  ");
//
//        SubjectPublicKeyInfo subjectPublicKeyInfo = SubjectPublicKeyInfo.getInstance(publicKey.getPublic().getEncoded());
//        X509v1CertificateBuilder v1CertGen = new X509v1CertificateBuilder(x500Name
//                , BigInteger.valueOf(random.nextLong())
//                , startDate
//                , endDate
//                , x500Name
//                , subjectPublicKeyInfo);
//
//        // Prepare Signature:
//        ContentSigner sigGen = null;
//        try
//        {
//            Security.addProvider(new BouncyCastleProvider());
//            sigGen = new JcaContentSignerBuilder("SHA256WithRSAEncryption").setProvider("SC").build(publicKey.getPrivate());
//// Self sign :
//            X509CertificateHolder x509CertificateHolder = v1CertGen.build(sigGen);
//            byte[] s = x509CertificateHolder.getEncoded();
//            log("x509CertificateHolder.getEncoded:  " + x509CertificateHolder.getEncoded());
//            String pubKeyStr = new String(Base64.encode(x509CertificateHolder.getEncoded(), Base64.DEFAULT));
//
//            PemObject pemObject = new PemObject("CERTIFICATE", x509CertificateHolder.toASN1Structure().getEncoded());
//            String pemObjectStr = new String(Base64.encode(pemObject.getContent(), Base64.DEFAULT));
//            log("pemObjectStr " + pemObjectStr + "test");
//
//
//            log("x509CertificateHolder.getEncoded:  " + pubKeyStr);
//            log("x509CertificateHolder. X509V2AttributeCertificate:  " + new X509V2AttributeCertificate(x509CertificateHolder.getEncoded()));
//            try
//            {
//                X509Certificate a = new JcaX509CertificateConverter().getCertificate(x509CertificateHolder);
//            }
//            catch (CertificateException error)
//            {
//            }
//
//
//        }
//        catch (OperatorCreationException e)
//        {
//            logError("x509CertificateHolder1 error ", e);
//
//        }
//        catch (IOException e)
//        {
//            logError("x509CertificateHolder2 error ", e);
//
//        }
//
//    }


//    @SuppressLint("NewApi")
//    public static String decryptRSAToString2(String encryptedBase64, String privateKey, String tranf)
//    {
//
//        try
//        {
//            // --- decrypt given OAEPParameterSpec
//            Cipher oaepFromInit = Cipher.getInstance(tranf);
//            OAEPParameterSpec oaepParams = new OAEPParameterSpec("SHA-256", "MGF1", new MGF1ParameterSpec("SHA-1"), PSource.PSpecified.DEFAULT);
//
//
//            KeyPairGenerator kpg = KeyPairGenerator.getInstance("RSA/ECB/PKCS1Padding");
//            kpg.initialize(256);
//            KeyPair kp = kpg.generateKeyPair();
//
//
//            oaepFromInit.init(Cipher.DECRYPT_MODE, kp.getPrivate(), oaepParams);
//
//
//            byte[] encryptedBytes = Base64.decode(encryptedBase64, Base64.DEFAULT);
//            byte[] pt = oaepFromInit.doFinal(encryptedBytes);
//            System.out.println(new String(pt, StandardCharsets.UTF_8));
//
//            return new String(pt, StandardCharsets.UTF_8);
//        }
//        catch (Exception e)
//        {
//            logError("decript error 2", e);
//        }
//        return "N/A1";
//    }
//
//
//    public static String decryptString(String base64String, String privateK, String transf)
//    {
//        try
//        {
////        FileInputStream is = new FileInputStream("priv.p12");
////        KeyStore keystore = KeyStore.getInstance("PKCS12");
////        keystore.load(is, "".toCharArray());
////        System.out.println("Successfully loaded");
//
//
//            String keyAlias = "1";
//
//
////        PrivateKey key = (PrivateKey)keystore.getKey(keyAlias, "".toCharArray());
//
////        System.out.println("key " + Base64.encode(key.getEncoded()));
//
//
//            KeyFactory keyFac = KeyFactory.getInstance("RSA/ECB/PKCS1Padding");
//            KeySpec keySpec = new PKCS8EncodedKeySpec(Base64.decode(privateK.trim().getBytes(), Base64.DEFAULT));
//            Key key = keyFac.generatePrivate(keySpec);
//
//
//            Cipher rsaDecryptCipher;
//            rsaDecryptCipher = Cipher.getInstance(transf);
//            rsaDecryptCipher.init(Cipher.DECRYPT_MODE, key);
//            final byte[] plainText;
//
//            plainText = rsaDecryptCipher.doFinal(Base64.decode(base64String, Base64.DEFAULT));
//            log("Plain   : " + new String(plainText));
//            return base64String;
//        }
//        catch (Exception e)
//        {
//
//        }
//
//        return " N/A";
//    }

    //--------------------- ENCRypt ANSWER ----------------//
    public static void encryptAnswerWithAES(int length, String secret, String algorithm) throws UnsupportedEncodingException
    {
        try
        {
            byte[] key = new byte[length];
            key = fixSecret(secret, length);
            SecretKeySpec secretKey = new SecretKeySpec(key, algorithm);

            Cipher cipher = Cipher.getInstance(algorithm);
        }
        catch (NoSuchAlgorithmException e)
        {
            e.printStackTrace();
        }
        catch (NoSuchPaddingException e)
        {
            e.printStackTrace();
        }
    }

    private static byte[] fixSecret(String s, int length) throws UnsupportedEncodingException
    {
        if (s.length() < length)
        {
            int missingLength = length - s.length();
            for (int i = 0; i < missingLength; i++)
            {
                s += " ";
            }
        }
        return s.substring(0, length).getBytes("UTF-8");
    }


    public static String encryptMsg(String message, SecretKey secret)
            throws NoSuchAlgorithmException, NoSuchPaddingException, InvalidKeyException, InvalidParameterSpecException, IllegalBlockSizeException, BadPaddingException, UnsupportedEncodingException
    {
   /* Encrypt the message. */
        Cipher cipher = null;
        cipher = Cipher.getInstance("AES/ECB/PKCS5Padding");
        cipher.init(Cipher.ENCRYPT_MODE, secret);
        byte[] cipherText = cipher.doFinal(message.getBytes("UTF-8"));
        return new String(Base64.encode(cipherText, Base64.DEFAULT));
    }

    public static String decryptMsg(byte[] cipherText, SecretKey secret)
            throws NoSuchPaddingException, NoSuchAlgorithmException, InvalidParameterSpecException, InvalidAlgorithmParameterException, InvalidKeyException, BadPaddingException, IllegalBlockSizeException, UnsupportedEncodingException
    {
    /* Decrypt the message, given derived encContentValues and initialization vector. */
        Cipher cipher = null;
        cipher = Cipher.getInstance("AES/ECB/PKCS5Padding");
        cipher.init(Cipher.DECRYPT_MODE, secret);
        String decryptString = new String(cipher.doFinal(cipherText), "UTF-8");
        return decryptString;
    }


//    public static void test5() throws Exception
//    {
//
//        Security.addProvider(new BouncyCastleProvider());
//
//        byte[] input = AppConstants.ENCR_TEST.getBytes();
//        Cipher cipher = Cipher.getInstance(AppConstants.TRANS, "BC");
////        SecureRandom random = new SecureRandom();
////        KeyPairGenerator generator = KeyPairGenerator.getInstance("RSA", "BC");
////
////        generator.initialize(256, random);
////
////        KeyPair pair = generator.generateKeyPair();
////        Key pubKey = pair.getPublic();
////        Key privKey = pair.getPrivate();
//
////        cipher.init(Cipher.ENCRYPT_MODE, pubKey, random);
////        byte[] cipherText = cipher.doFinal(input);
////        log("test5 cipher: " + new String(cipherText));
//
//
//        cipher.init(Cipher.DECRYPT_MODE, stringToPrivateKey());
//        byte[] plainText = cipher.doFinal(input);
//        log("test5 plain : " + new String(plainText));
//    }


    public static void test4(String encodedStringFromContainer) throws IOException
    {
//        '/' and '+' to '_' and '-'
        encodedStringFromContainer = encodedStringFromContainer.replace("/", "_");
        encodedStringFromContainer = encodedStringFromContainer.replace("+", "-");

        Security.addProvider(new BouncyCastleProvider());
        String privateKeyString = PreferencesManager.getPrivatePem();
//        "-----BEGIN RSA PRIVATE KEY-----\n" + "MIICXQIBAAKBgQDKQtJAyCu5FHwDncK2LB/J5ClJhulGggyc7vwtji6TJHtSJfgD\n" + "4TLpHRIHh/cHqf3brhpQtYB9yjKlwogji/OzedY2mdTdSOP8O6suJYu3QENN2xG/\n" + "HvT8UiYK3feVLbJtukhJm7eSuwfMDsjHh4AK7g11fVs6EmY+foh3mjoKLQIDAQAB\n" + "AoGAR8N/wDaFtOx8t/fAv0xWlxaaQ5lXqYm5GfF9jlhVVCXsj5AjOJUtsCJ9ZCis\n" + "0I5TIR/b/Gj5xyf34nJsRViBxbnf6XdLGyXmzsNxWZoWbM70JaqU3iQKm605/EnD\n" + "vPgrI0AMfc/h6Kog0zLrKWKkna+wE5839yMmm7WPqgvxSc0CQQDoud5e3yZu/1e+\n" + "7piFZZl6StAecl+k10Wq5kzJeVQRffDB3JCca65H/W1EZIzEh76pUNr7SYAIIcbK\n" + "jzOdbj1vAkEA3n0AudM3mBzklLEUSHs1ZSqFkUMNP9MNIikwkZ/9Z2AlhW5gnwiv\n" + "dgeXonTqlTFux4e7uyKZoJpJcKAgmMicIwJBAIMl206TalE6y/Po+UKTUr470rSV\n" + "t5hpR/Va+wK+wMVqt3ZIGaZMeFZRVnYoQ7us06EO05iwftoWTrRvpqKdMTkCQBkE\n"
//                + "QzWhy0l+TjFt69Luj6Vtb5FS0cWQbJSfvwdQzwR1qiJjs9eN+XSzC9jHfq0B3uvu\n" + "lixHirClSIayapfjTrMCQQCM8d97py4u9hCdCpsHBDt54dXkHsDA2abNzaPri/YA\n" + "pNFZGrfXKVGSLFOfsuf7Wj+yL7ew6ZVKOMYdJ+zb9Wwv\n" + "-----END RSA PRIVATE KEY-----"; // 128
        // bit
        // key
        String publicKeyString = PreferencesManager.getPublicPem();
//        "-----BEGIN PUBLIC KEY-----\n" + "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDKQtJAyCu5FHwDncK2LB/J5ClJ\n" + "hulGggyc7vwtji6TJHtSJfgD4TLpHRIHh/cHqf3brhpQtYB9yjKlwogji/OzedY2\n" + "mdTdSOP8O6suJYu3QENN2xG/HvT8UiYK3feVLbJtukhJm7eSuwfMDsjHh4AK7g11\n" + "fVs6EmY+foh3mjoKLQIDAQAB\n" + "-----END PUBLIC KEY-----";
        String message = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.";
        String cypherText = "EqVFWCMJ2rSy1J0PjAkRRZKkQ24TJ7xQi%2FjKUa3E7ZJ%2FlwtFsBkUDqJ9VUb0aC53O4TM4uNKMmYQNFDTHpQSgoun95ExgoCAvC1BXz2jVzWkKavt1vWbhS1C5VKcWU0hfUOmxZgiOT4rGWpEXVXoLodKLiJnbkvVNZyjgw0LZPQ%3D";

        System.out.println("11122 private:");
        System.out.println("11122 encodedStringFromContainer :" + encodedStringFromContainer);

        Reader privateKeyReader = new StringReader(privateKeyString);

        PEMParser privatePemParser = new PEMParser(privateKeyReader);
        Object privateObject = privatePemParser.readObject();
        System.out.println("11122 private: " + privateObject.getClass());
        if (privateObject instanceof PEMKeyPair)
        {
            PEMKeyPair pemKeyPair = (PEMKeyPair) privateObject;
            System.out.println("11122 private: " + pemKeyPair.getPrivateKeyInfo());
            System.out.println("11122 public: " + pemKeyPair.getPublicKeyInfo());

            JcaPEMKeyConverter converter = new JcaPEMKeyConverter().setProvider("BC");

            PublicKey publicKey = converter.getPublicKey(pemKeyPair.getPublicKeyInfo());
            PrivateKey privatekey = converter.getPrivateKey(pemKeyPair.getPrivateKeyInfo());

//            String encodedURL = null;
//            try
//            {
//                System.out.println("11122 encrypting using WP publicKey and own cypherText\r\n");
//
//                byte[] encripted = encrypt(publicKey, message);
//                System.out.println("11122 encrypted: " + new String(encripted));
//
//                byte[] encodedURLBase64 = Base64.encode(encripted, Base64.DEFAULT);
//                System.out.println("11122 base64: " + new String(encodedURLBase64));
//
//                encodedURL = URLEncoder.encode(new String(encodedURLBase64));
//                System.out.println("11122 encodedURL: " + encodedURL);
//            }
//            catch (Exception erm)
//            {
//                System.out.println("11122 erm: " + erm.getMessage());
//            }
            // and back
            try
            {
                System.out.println("11122333 decrypting using WP publicKey and own cypherText\r\n");
                String decodedURL = URLDecoder.decode(encodedStringFromContainer);
                System.out.println("11122333 decodedURL: " + decodedURL);

                byte[] decodedURLBase64 = Base64.decode(decodedURL, Base64.URL_SAFE);
                System.out.println("11122333 decodedURLBase64: " + new String(decodedURLBase64));

                String decrypted = decrypt(privatekey, decodedURLBase64);
                System.out.println("11122333 decrypted: " + new String(decrypted));
            }
            catch (Exception erm)
            {
                logError("11122333 erm: ", erm);
            }
            // using stuff from external party
            try
            {
                System.out.println("11122 decrypting using WP publicKey and WP cypherText\r\n");

                String decodedURLWP = URLDecoder.decode(cypherText);
                System.out.println("11122 decodedURLBase64WP: " + decodedURLWP);

                byte[] decodedURLBase64WP = Base64.decode(decodedURLWP, Base64.URL_SAFE);
                System.out.println("11122 decodedURLBase64WP: " + new String(decodedURLBase64WP));

                String decryptedWP = decrypt(privatekey, decodedURLBase64WP);
                System.out.println("11122 decryptedWP1: " + decryptedWP);
                System.out.println("11122 decryptedWP2: " + new String(decryptedWP));
            }
            catch (Exception erm)
            {
                logError("11122 erm22: ", erm);
            }

        }
        privatePemParser.close();

        System.out.println("11122 public:");
        Reader publicKeyReader = new StringReader(publicKeyString);
        PEMParser publicPemParser = new PEMParser(publicKeyReader);

        Object publicObject = publicPemParser.readObject();
        System.out.println("11122 public: " + publicObject.getClass());
        if (publicObject instanceof SubjectPublicKeyInfo)
        {
            SubjectPublicKeyInfo publicSubjectPublicKeyInfo = (SubjectPublicKeyInfo) publicObject;
            // System.out.println("private: "+publicSubjectPublicKeyInfo);
            System.out.println("11122 public: " + publicSubjectPublicKeyInfo);

        }
        publicPemParser.close();


    }

    private static byte[] encrypt(Key pubkey, String text)
    {
        try
        {
            Cipher rsa;
            rsa = Cipher.getInstance("RSA", "BC");
            rsa.init(Cipher.ENCRYPT_MODE, pubkey);
            return rsa.doFinal(text.getBytes());
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        return null;
    }

    private static String decrypt(Key decryptionKey, byte[] buffer)
    {
        try
        {
            Cipher rsa;
            rsa = Cipher.getInstance("RSA", "BC");
            rsa.init(Cipher.DECRYPT_MODE, decryptionKey);
            byte[] utf8 = rsa.doFinal(buffer);
            return new String(utf8);
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
        return null;
    }


    /**
     * RSA encrypt function (RSA / ECB / PKCS1-Padding)
     *
     * @param original
     * @param key
     * @return
     */
    public static byte[] rsaEncrypt(byte[] original, PublicKey key)
    {
        try
        {
            Cipher cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding");
            cipher.init(Cipher.ENCRYPT_MODE, key);
            return cipher.doFinal(original);
        }
        catch (Exception e)
        {
            //  Logger.e(e.toString());
        }
        return null;
    }

    /**
     * RSA decrypt function (RSA / ECB / PKCS1-Padding)
     *
     * @param encrypted
     * @param key
     * @return
     */
    public static byte[] rsaDecrypt(byte[] encrypted, PrivateKey key)
    {
        try
        {
            Cipher cipher = Cipher.getInstance("RSA/ECB/PKCS1Padding");
            cipher.init(Cipher.DECRYPT_MODE, key);
            return cipher.doFinal(encrypted);
        }
        catch (Exception e)
        {
            //  Logger.e(e.toString());
        }
        return null;
    }


    private static byte[] encrypt(byte[] raw, byte[] clear) throws Exception
    {
        SecretKeySpec skeySpec = new SecretKeySpec(raw, "AES");
        Cipher cipher = Cipher.getInstance("AES");
        cipher.init(Cipher.ENCRYPT_MODE, skeySpec);
        byte[] encrypted = cipher.doFinal(clear);
        return encrypted;
    }

    private static byte[] decrypt(byte[] raw, byte[] encrypted) throws Exception
    {
        SecretKeySpec skeySpec = new SecretKeySpec(raw, "AES");
        Cipher cipher = Cipher.getInstance("AES");
        cipher.init(Cipher.DECRYPT_MODE, skeySpec);
        byte[] decrypted = cipher.doFinal(encrypted);
        return decrypted;
    }

    public static String invoceEnc(String answer, String secret)
    {

        byte[] encodedKey = Base64.decode(secret, Base64.DEFAULT);
        SecretKey originalKey = new SecretKeySpec(encodedKey, 0, encodedKey.length, "AES");

        try
        {
            byte[] key = originalKey.getEncoded();

            // encrypt
            byte[] encryptedData = encrypt(key, answer.getBytes());

            return new String(encryptedData);

            // decrypt
//            byte[] decryptedData = decrypt(key, encryptedData);


        }
        catch (Exception e)
        {
            logError("test55 ", e);
        }
        return answer;
    }

    public static String decrypt11(String encrypted, String simetric, String ivString)
    {
        try
        {
            IvParameterSpec iv = new IvParameterSpec(ivString.getBytes("UTF-8"));
            SecretKeySpec skeySpec = new SecretKeySpec(simetric.getBytes("UTF-8"), "AES");
            Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5PADDING");
            cipher.init(Cipher.DECRYPT_MODE, skeySpec, iv);
            byte[] original = cipher.doFinal(Base64.decode(encrypted, Base64.DEFAULT));
            return new String(original);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return null;
    }

    public static String encrypt11(String value, String secret)
    {
        try
        {
            String INIT_VECTOR = "1234567890123456";
            IvParameterSpec iv = new IvParameterSpec(INIT_VECTOR.getBytes("UTF-8"));
            SecretKeySpec skeySpec = new SecretKeySpec(secret.getBytes("UTF-8"), "AES");
            Cipher cipher = Cipher.getInstance("AES/CBC/PKCS5PADDING");
            cipher.init(Cipher.ENCRYPT_MODE, skeySpec, iv);
            byte[] encrypted = cipher.doFinal(value.getBytes());
            System.out.println("encrypted string: " + Base64.encode(encrypted, Base64.DEFAULT));
            return new String(Base64.encode(encrypted, Base64.DEFAULT));
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
        return null;
    }
}


