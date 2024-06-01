import java.util.Random;
import java.util.Scanner;

public class Bot {
    private static final String[] CPFS = {
        "12345678901", "98765432109", "11122233344", "55566677788", "99900011122",
        "33344455566", "77788899900", "22233344455", "66677788899", "00011122233"
    };
    private static final int[] PRODUCT_IDS_CASE1 = {1, 2, 3, 4, 5};
    private static final int[] PRODUCT_IDS_CASE2 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    private static final int[] PRODUCT_IDS_CASE3 = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.println("Digite 1 para o caso 1, 2 para o caso 2, ou 3 para o caso 3:");

        int caseNumber = scanner.nextInt();
        if (caseNumber == 1) {
            executeCase1();
        } else if (caseNumber == 2) {
            executeCase2();
        } else if (caseNumber == 3) {
            executeCase3();
        } else {
            System.out.println("Saindo...");
        }

        scanner.close();
    }

    private static void executeCase1() {
        Random random = new Random();

        for (int i = 0; i < 2; i++) {
            String cpf = CPFS[random.nextInt(CPFS.length)];
            StringBuilder cart = new StringBuilder();
            cart.append(cpf).append(" ");

            int numProducts = random.nextInt(3) + 2;

            for (int j = 0; j < numProducts; j++) {
                int productId = PRODUCT_IDS_CASE1[random.nextInt(PRODUCT_IDS_CASE1.length)];
                cart.append(productId).append(" 1 ");
            }

            String command = "comprar " + cart.toString().trim();
            new Thread(() -> new Cliente().enviarComando(command)).start();
        }
    }

    private static void executeCase2() {
        Random random = new Random();

        for (int i = 0; i < 10; i++) {
            String cpf = CPFS[random.nextInt(CPFS.length)];
            StringBuilder cart = new StringBuilder();
            cart.append(cpf).append(" ");

            int numProducts = random.nextInt(3) + 2;

            for (int j = 0; j < numProducts; j++) {
                int productId = PRODUCT_IDS_CASE2[random.nextInt(PRODUCT_IDS_CASE2.length)];
                cart.append(productId).append(" 1 ");
            }

            String command = "comprar " + cart.toString().trim();
            new Thread(() -> new Cliente().enviarComando(command)).start();
        }
    }

    private static void executeCase3() {
        Random random = new Random();

        for (int i = 0; i < 1000; i++) {
            String cpf = CPFS[random.nextInt(CPFS.length)];
            StringBuilder cart = new StringBuilder();
            cart.append(cpf).append(" ");

            int productId = PRODUCT_IDS_CASE3[random.nextInt(PRODUCT_IDS_CASE3.length)];
            cart.append(productId).append(" 1 ");

            String command = "comprar " + cart.toString().trim();
            new Thread(() -> new Cliente().enviarComando(command)).start();
        }
    }
}
