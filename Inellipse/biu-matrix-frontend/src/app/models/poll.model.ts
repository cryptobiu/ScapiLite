export interface Poll {
  id: string;
  name: string;
  title: string;
  description: string;
  executionTime: number;
  active: boolean;
  userRegistrationSecondsBeforeExecution: number;
  resultType: string;
  closedForRegistration: boolean;
}
