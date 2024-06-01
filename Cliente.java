import java.io.*;
import java.net.*;
import java.util.Scanner;

public class Cliente {
    private static final String ENDERECO_SERVIDOR = "25.58.164.23";
    private static final int PORTA_SERVIDOR = 8080;

    public static void main(String[] args) {
        Cliente cliente = new Cliente();
        cliente.executar();
    }

    public void executar() {
        Scanner scanner = new Scanner(System.in);
        while (true) {
            try {
				Thread.sleep(1000);
                System.out.println("Digite 'estoque' para listar produtos, 'comprar' para iniciar uma compra, 'relatorio' para buscar um log de compra, ou 'sair' para sair:");
                String comandoUsuario = scanner.nextLine();

                if (comandoUsuario.equalsIgnoreCase("estoque")) {
                    new Thread(() -> enviarComando(comandoUsuario)).start();
                } else if (comandoUsuario.equalsIgnoreCase("comprar")) {
                    System.out.print("Digite o CPF: ");
                    String cpf = scanner.nextLine();
                    System.out.println("__");
                    System.out.println("  \\______________________");
                    System.out.println("  |                      |");
                    System.out.println("  |                      |");
                    System.out.println("  |                      |");
                    System.out.println("  |______________________|");
                    System.out.println("      O            O");
                    StringBuilder carrinho = new StringBuilder();
                    carrinho.append(cpf).append(" ");

                    while (true) {
                        System.out.print("Digite o ID do produto e a quantidade (ou 'fim' para finalizar): ");
                        String entradaProduto = scanner.nextLine();

                        if (entradaProduto.equalsIgnoreCase("fim")) {
                            break;
                        }

                        carrinho.append(entradaProduto).append(" ");
                    }

                    new Thread(() -> enviarComando("comprar " + carrinho.toString().trim())).start();
                } else if (comandoUsuario.equalsIgnoreCase("relatorio")) {
                    System.out.print("Digite o CPF: ");
                    String cpf = scanner.nextLine();
                    System.out.print("Digite o ID da compra: ");
                    int idCompra = scanner.nextInt();
                    scanner.nextLine(); // Consumir nova linha
                    new Thread(() -> enviarComando("relatorio " + cpf + " " + idCompra)).start();
                } else if (comandoUsuario.equalsIgnoreCase("sair")) {
                    break;
                } else {
                    System.out.println("Comando desconhecido. Tente novamente.");
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        scanner.close();
    }

    public void enviarComando(String comando) {
        try {
            Socket socket = new Socket();
            socket.connect(new InetSocketAddress(ENDERECO_SERVIDOR, PORTA_SERVIDOR), 10000);
            socket.setSoTimeout(10000);

            PrintWriter saida = new PrintWriter(socket.getOutputStream(), true);
            BufferedReader entrada = new BufferedReader(new InputStreamReader(socket.getInputStream()));

            saida.println(comando);
            lerResposta(entrada);

            entrada.close();
            saida.close();
            socket.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void lerResposta(BufferedReader entrada) throws IOException {
        String resposta;
        StringBuilder respostaCompleta = new StringBuilder();

        while ((resposta = entrada.readLine()) != null && !resposta.isEmpty()) {
            respostaCompleta.append(resposta).append("\n");
        }

        System.out.println(respostaCompleta.toString());
    }
}
