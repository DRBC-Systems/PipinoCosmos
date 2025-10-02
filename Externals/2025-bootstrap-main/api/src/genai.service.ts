import { Injectable } from '@nestjs/common';
import { BedrockEmbeddings, ChatBedrockConverse } from "@langchain/aws";
import { IterableReadableStream } from "@langchain/core/dist/utils/stream";
import { AIMessageChunk } from "@langchain/core/messages";
import { RecursiveCharacterTextSplitter } from "@langchain/textsplitters";
import { MemoryVectorStore } from "langchain/vectorstores/memory";
import { TextLoader } from "langchain/document_loaders/fs/text";
import { Document } from "@langchain/core/documents";
import { ChatPromptTemplate } from "@langchain/core/prompts";
import { pull } from "langchain/hub";
import { Annotation, StateGraph } from "@langchain/langgraph";
import { ChatDto } from "./chat.dto";
import { TYPES } from "./message.dto";

@Injectable()
export class GenAIService {
  static GENAI_MODEL = "anthropic.claude-3-5-sonnet-20240620-v1:0";
  static EMBEDDINGS_MODEL = "amazon.titan-embed-text-v2:0";

  getGenAIModel() {
    return new ChatBedrockConverse({
      region: process.env.AWS_REGION ?? "us-east-1",
      model: GenAIService.GENAI_MODEL,
      credentials: {
        secretAccessKey: process.env.AWS_SECRET_ACCESS_KEY ?? "",
        accessKeyId: process.env.AWS_ACCESS_KEY_ID ?? "",
        sessionToken: process.env.AWS_SESSION_TOKEN ?? "",
      },
    });
  }

  getEmbeddingsModel() {
    return new BedrockEmbeddings({
      maxRetries: 0,
      region: process.env.AWS_REGION ?? "us-east-1",
      model: GenAIService.EMBEDDINGS_MODEL,
      credentials: {
        secretAccessKey: process.env.AWS_SECRET_ACCESS_KEY ?? "",
        accessKeyId: process.env.AWS_ACCESS_KEY_ID ?? "",
        sessionToken: process.env.AWS_SESSION_TOKEN ?? "",
      },
    });
  }

  async prompt(message: string): Promise<string | null> {
    try {
      const model = this.getGenAIModel();
      const response = await model.invoke(message);

      return response.text;
    } catch ( err ) {
      console.log("Error in GenAIService.prompt:", err);
      return null;
    }
  }

  async stream(message: string): Promise<IterableReadableStream<AIMessageChunk> | null> {
    try {
      const model = this.getGenAIModel();
      return await model.stream(message);
    } catch ( err ) {
      console.log("Error in GenAIService.stream:", err);
      return null;
    }
  }

  async chat(chatDto: ChatDto): Promise<string | null> {
    try {
      const model = this.getGenAIModel();
      const input: any[] = [];

      for (const message of chatDto.messages) {
        input.push({role: message.type == TYPES.HUMAN ? 'user' : 'assistant', content: message.text});
      }

      const response = await model.invoke(input);

      return response.text;
    } catch ( err ) {
      console.log("Error in GenAIService.chat:", err);
      return null;
    }
  }

  async embeddings(message: string): Promise<string | null> {
    try {
      const model = this.getEmbeddingsModel();
      const res = await model.embedQuery(message);

      // Output only the first embedding for brevity
      return '[ ' + res[0].toString() + ', ... ]';
    } catch ( err ) {
      console.log("Error in GenAIService.embeddings:", err);
      return null;
    }
  }

  async rag(message: string): Promise<string | null> {
    try {
      // Create the models (LLM, embeddings)
      const llm = this.getGenAIModel();
      const embeddings = this.getEmbeddingsModel();

      // Load the document and create the embeddings
      const vectorStore = new MemoryVectorStore(embeddings);

      const docLoader = new TextLoader(process.cwd() + '/data/AliceAdventuresInWonderland.txt');
      const docs = await docLoader.load();

      const splitter = new RecursiveCharacterTextSplitter({
        chunkSize: 1000, chunkOverlap: 200
      });
      const allSplits = await splitter.splitDocuments(docs);
      await vectorStore.addDocuments(allSplits);

      // Prepare the Graph to process the incoming query
      const promptTemplate = await pull<ChatPromptTemplate>("rlm/rag-prompt");

      const InputStateAnnotation = Annotation.Root({
        question: Annotation<string>,
      });

      const StateAnnotation = Annotation.Root({
        question: Annotation<string>,
        context: Annotation<Document[]>,
        answer: Annotation<string>,
      });

      const retrieve = async (state: typeof InputStateAnnotation.State) => {
        const retrievedDocs = await vectorStore.similaritySearch(state.question)
        return { context: retrievedDocs };
      };

      const generate = async (state: typeof StateAnnotation.State) => {
        const docsContent = state.context.map(doc => doc.pageContent).join("\n");
        const messages = await promptTemplate.invoke({ question: state.question, context: docsContent });
        const response = await llm.invoke(messages);
        return { answer: response.content };
      };

      const graph = new StateGraph(StateAnnotation)
          .addNode("retrieve", retrieve)
          .addNode("generate", generate)
          .addEdge("__start__", "retrieve")
          .addEdge("retrieve", "generate")
          .addEdge("generate", "__end__")
          .compile();

      // Invoke the graph and get the result
      const result = await graph.invoke({ question: message });

      console.log(`\nCitations: ${result.context.length}`);
      console.log(result.context.slice(0, 2));

      return result.answer;
    } catch ( err ) {
      console.log("Error in GenAIService.rag:", err);
      return null;
    }
  }
}
